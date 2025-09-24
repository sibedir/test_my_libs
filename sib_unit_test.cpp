#include "sib_unit_test.h"

#include <mutex>

namespace sib {
namespace debug {

    static bool is_initialized_val = false;

    bool const & is_initialized = is_initialized_val;

    bool Init()
    {
        ::sib::console::Init();

        if (is_initialized_val) return true;
        
        debugging_reactions_to_keys[::sib::console::KC_ESC  ] = []() { throw EDebugAbort("");           };
        debugging_reactions_to_keys[::sib::console::KC_ENTER] = []() {                                  };
        debugging_reactions_to_keys[::sib::console::KC_F5   ] = []() { current_break_level = BP_CUSTOM; };
        debugging_reactions_to_keys[::sib::console::KC_F10  ] = []() { current_break_level = BP_BEGIN;  };
        debugging_reactions_to_keys[::sib::console::KC_F11  ] = []() { current_break_level = BP_END;    };
        debugging_reactions_to_keys[::sib::console::KC_F12  ] = []() { current_break_level = BP_ALL;    };
        
        return is_initialized_val = true;
    }

    ::std::mutex mtx{};

    void under_lock_print(string const& str)
    {
        ::std::lock_guard lock(mtx);
        outstream << str;
        outstream.flush();
    }



    // ----------------------------------------------------------------------------------- debug tests

    void RunAllTest() { for (auto& it: Tests) it.second.run(); }


    
    // ----------------------------------------------------------------------------------- TTestLogRec

    string TTestLogRec::united_message() const
    {
        TBufer res;
        auto i = static_cast<int>(typ);
        res << "[" << test_log_type_name[i] << "] " << str;
        return res.str();
    }


    
    // ----------------------------------------------------------------------------------- TTest
    
    const TTestState & TTest::state() const { return _state; }
    const TTestLog   & TTest::log  () const { return _log  ; }
    const TTestFunc  & TTest::test () const { return _test ; }
    
    void TTest::message(string && str) { write_to_log(TTestLogType::message , ::std::move(str)); }
    void TTest::warning(string && str) { write_to_log(TTestLogType::warning , ::std::move(str)); }
    void TTest::error  (string && str) { write_to_log(TTestLogType::error   , ::std::move(str)); }
    
    void TTest::write_to_log(TTestLogType st, string && str) { _log.emplace_back(st, str); }
    
    void TTest::run()
    {
        _state = TTestState::NotInitialized;
        try
        {   
            _log.clear();
            _state = TTestState::NotCompleted;
            
            BEG_COUNTR = 0;
            int res = _test(_log);
            
            auto str = "Return: " + ::std::to_string(res);
            if (res != 0) error  (str);
            else          message(str);
            _state = TTestState::Completed;
        }
        catch (std::exception const & e)
        {
            string what = e.what();
            if (what != string()) { what = string(":\n") + what; }
            error(string("Test stopped due to exception [", typeid(e).name(), "]", what));
        }
        catch (...)
        {
            error("Test stopped due to unknown exception!");
        }
    }
    

    // ----------------------------------------------------------------------------------- debugging step by step
    
    void SetBreakPoint(TBreakPointLevel bp_level /*= BP_CUSTOM*/, string msg /*= {}*/)
    {
        ::std::set<::sib::console::TKeyCode> debugging_keys;
        for (auto it = debugging_reactions_to_keys.begin(); it != debugging_reactions_to_keys.end(); ++it)
        {
            debugging_keys.insert(it->first);
        }
        
        if (current_break_level > bp_level) return;
        if (current_break_level == BP_END and bp_level == BP_BEGIN) return;
        
        if (bp_level == BP_CUSTOM)
        {
            if (msg != string()) { under_lock_print(msg + string("\n")); }
            else { under_lock_print("       - break point -       [Enter] - continue   [Esc] - abort\n"); }
        }
        
        WaitReactToKeyCodes(debugging_keys, debugging_reactions_to_keys);
    }
    
} // namespace debug 
} // namespace sib
