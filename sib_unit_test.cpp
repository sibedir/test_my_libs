#include "sib_unit_test.h"

TBreakPointLevel current_break_level = BP_CUSTOM;

static sib::Tconsole_reactions_to_keys debugging_reactions_to_keys{};

bool const is_initialized_unit_test_module = []()
    {
        debugging_reactions_to_keys[sib::KC_ESC  ] = []() { current_break_level = BP_NONE;   };
        debugging_reactions_to_keys[sib::KC_ENTER] = []() {                                  };
        debugging_reactions_to_keys[sib::KC_F5   ] = []() { current_break_level = BP_CUSTOM; };
        debugging_reactions_to_keys[sib::KC_F10  ] = []() { current_break_level = BP_BEGIN;  };
        debugging_reactions_to_keys[sib::KC_F11  ] = []() { current_break_level = BP_END;    };
        debugging_reactions_to_keys[sib::KC_F12  ] = []() { current_break_level = BP_ALL;    };
        return true;
    }
();

inline void SetBreakPoint(TBreakPointLevel bp_level)
{
    static std::set<sib::TKeyCode> const debugging_keys = []()
        {
            std::set<sib::TKeyCode> res;
            for (auto it = debugging_reactions_to_keys.begin(); it != debugging_reactions_to_keys.end(); ++it)
            {
                res.insert(it->first);
            }        
            return res;
        }
    ();


    if (current_break_level > bp_level) return;
    if (current_break_level == BP_END and bp_level == BP_BEGIN) return;

    if (bp_level == BP_CUSTOM) { std::cout << "[ break point ]\n"; }

    sib::WaitReactToKeyCodes(debugging_keys, debugging_reactions_to_keys);
}
