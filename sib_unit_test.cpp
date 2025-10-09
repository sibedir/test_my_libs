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

    void under_lock_print(TString const& str)
    {
        ::std::lock_guard lock(mtx);
        outstream << str;
        outstream.flush();
    }



    // ----------------------------------------------------------------------------------- debug tests

    void RunAllTest()
    {
        for (auto& it: Tests) it.second.run();
    }

    TString ReportText()
    {
        TBufer buf;
        TString border = "********************************************************************************************************\n";
        buf << border << "                                                REPORT                                                  \n";
        for (auto test_it : Tests)
        {
            buf << border;
            buf << "TEST: " << test_it.first << "\n";
            auto& test = test_it.second;

            switch (test.state()) {
            case TTestState::NotInitialized: buf << "Not initialized" << "\n"; break;
            case TTestState::NotCompleted: buf << "Not completed" << "\n"; break;
            case TTestState::Completed: buf << "Completed" << "\n"; break;
            default: buf << "Unknown state" << "\n";
            }

            int l = 0, m = 0, w = 0, e = 0;
            for (auto rec_it : test.log()) {
                buf << rec_it.united_message() << "\n";
                ++l;
                switch (rec_it.typ) {
                    case TTestLogType::message: ++m; break;
                    case TTestLogType::warning: ++w; break;
                    case TTestLogType::error  : ++e; break;
                }
            }
            buf << "\nlog count: " << l
                << "  messages: "  << m
                << "  warnings: "  << w
                << "  errors: "    << e
                << "\n";

            border = "--------------------------------------------------------------------------------------------------------\n";
        }
        buf << "********************************************************************************************************\n";
        return buf.str();
    }


    
    // ----------------------------------------------------------------------------------- TTestLogRec

    TString TTestLogRec::united_message() const
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
    
    void TTest::message(TString && str) { write_to_log(TTestLogType::message , ::std::move(str)); }
    void TTest::warning(TString && str) { write_to_log(TTestLogType::warning , ::std::move(str)); }
    void TTest::error  (TString && str) { write_to_log(TTestLogType::error   , ::std::move(str)); }
    
    void TTest::write_to_log(TTestLogType st, TString && str) { _log.emplace_back(st, str); }
    
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
            TString what = e.what();
            if (what != TString()) { what = TString(":\n") + what; }
            error(TString("Test stopped due to exception [", typeid(e).name(), "]", what));
        }
        catch (...)
        {
            error("Test stopped due to unknown exception!");
        }
    }
    

    // ----------------------------------------------------------------------------------- debugging step by step
    
    void SetBreakPoint(TBreakPointLevel bp_level /*= BP_CUSTOM*/, TString msg /*= {}*/)
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
            if (msg != TString()) { under_lock_print(msg + TString("\n")); }
            else { under_lock_print("       - break point -       [Enter] - continue   [Esc] - abort\n"); }
        }
        
        WaitReactToKeyCodes(debugging_keys, debugging_reactions_to_keys);
    }
    
} // namespace debug 
} // namespace sib
