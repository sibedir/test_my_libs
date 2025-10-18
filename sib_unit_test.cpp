#include "sib_unit_test.h"

#include <mutex>
#include <iomanip>

namespace sib {
namespace debug {

    namespace detail {

        thread_local unsigned beg_accum = 0;
        thread_local unsigned lin_accum = 0;
        thread_local unsigned nes_accum = 0;

        thread_local TBufer output_bufer {};

    } // namespace detail

    static bool is_initialized_val = false;

    bool const & is_initialized = is_initialized_val;

    bool Init()
    {
        ::sib::console::Init();

        if (is_initialized_val) return true;
        
        debugging_reactions_to_keys[::sib::console::KC_ESC  ] = { "abort"        , [](){ throw EDebugAbort("");           } };
        debugging_reactions_to_keys[::sib::console::KC_ENTER] = { "continue"     , [](){ /*do {} while (0);*/                 }};
        debugging_reactions_to_keys[::sib::console::KC_F5   ] = { "to next BP"   , [](){ current_break_level = BP_CUSTOM; } };
        debugging_reactions_to_keys[::sib::console::KC_F10  ] = { "to BEG mark"  , [](){ current_break_level = BP_BEGIN;  } };
        debugging_reactions_to_keys[::sib::console::KC_F11  ] = { "to END mark"  , [](){ current_break_level = BP_END;    } };
        debugging_reactions_to_keys[::sib::console::KC_F12  ] = { "line by line" , [](){ current_break_level = BP_ALL;    } };

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
            buf << "  TEST: " << test_it.first << "\n";
            auto& test = test_it.second;

            switch (test.state()) {
            case TTestState::NotInitialized: buf << "  Not initialized" << "\n"; break;
            case TTestState::NotCompleted  : buf << "  Not completed"   << "\n"; break;
            case TTestState::Completed     : buf << "  Completed"       << "\n"; break;
            default: buf << "  Unknown state" << "\n";
            }

            int l = 0, m = 0, w = 0, e = 0;
            buf << "  ---------------------------------------------------\n"
                << "  | Type     | Blok | Line | Description\n"
                << "  ---------------------------------------------------\n";
            for (auto rec_it : test.log()) {
                auto mes = rec_it.united_message();
                buf << mes;
                if (*mes.rbegin() != '\n') buf << "\n";
                ++l;
                switch (rec_it.type) {
                    case TTestLogType::message: ++m; break;
                    case TTestLogType::warning: ++w; break;
                    case TTestLogType::error  : ++e; break;
                }
            }
            buf << "  ---------------------------------------------------\n";
            buf << "  log count: " << l
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
        auto tidx = static_cast<int>(type);
        res << std::left
            << "  | "
            << std::setw(8) << test_log_type_name[tidx]
            << " | "
            << std::setw(4) << (beg_num ? TString(beg_num) : TString())
            << " | "
            << std::setw(4) << (lin_num ? TString(lin_num) : TString())
            << " | ";

        if (!description.empty())
        {
            auto last = std::prev(description.end());
            for (auto it = description.begin(); it != description.end(); ++it) {
                res << *it;
                if ((*it != '\n') or (it == last)) continue;
                res << "  |          |      |      | ";
            }
        }
        return res.str();
    }


    
    // ----------------------------------------------------------------------------------- TTest
    
    const TTestState & TTest::state() const { return _state; }
    const TTestLog   & TTest::log  () const { return _log  ; }
    const TTestFunc  & TTest::test () const { return _test ; }
    
    void TTest::message(size_t beg_num, size_t lin_num, TString && str) { write_to_log(TTestLogType::message, beg_num, lin_num, std::move(str)); }
    void TTest::warning(size_t beg_num, size_t lin_num, TString && str) { write_to_log(TTestLogType::warning, beg_num, lin_num, std::move(str)); }
    void TTest::error  (size_t beg_num, size_t lin_num, TString && str) { write_to_log(TTestLogType::error  , beg_num, lin_num, std::move(str)); }
    
    void TTest::write_to_log(TTestLogType type, size_t beg_num, size_t lin_num, TString && str) { _log.emplace_back(type, beg_num, lin_num, str); }
    
    void TTest::run()
    {
        _state = TTestState::NotInitialized;
        try
        {   
            _log.clear();
            _state = TTestState::NotCompleted;
            
            detail::beg_accum = 0;
            detail::lin_accum = 0;
            detail::nes_accum = 0;

            //::sib::debug::detail::output_bufer
            //    << "****************************************************************************************************"
            //    <<             MSG("                                            sib_console                                             ");
            //MSG("****************************************************************************************************");
            //MSG("");
            //::sib::debug::detail::finish_macro(sib::debug::BP_ALL);

            int res = _test(_log);
            
            auto str = "Return: " + ::std::to_string(res);
            if (res != 0) error  (0, 0, str);
            else          message(0, 0, str);
            _state = TTestState::Completed;
        }
        catch (std::exception const & e)
        {
            TString what = e.what();
            if (what != TString()) { what = TString(":\n") + what; }
            error(BEG_ACCUM, LIN_ACCUM, TString("Test stopped due to exception [", typeid(e).name(), "]", what));
        }
        catch (...)
        {
            error(BEG_ACCUM, LIN_ACCUM, "Test stopped due to unknown exception!");
        }
    }
    

    // ----------------------------------------------------------------------------------- debugging step by step
    
    console::TKeyCode SetBreakPoint(TBreakPointLevel bp_level /*= BP_CUSTOM*/, TString msg /*= {}*/)
    {
        ::std::set<::sib::console::TKeyCode> debugging_keys;
        for (auto it = debugging_reactions_to_keys.begin(); it != debugging_reactions_to_keys.end(); ++it)
        {
            debugging_keys.insert(it->first);
        }
        
        if (current_break_level > bp_level) return console::KC_EMPTY;
        if (current_break_level == BP_END and bp_level == BP_BEGIN) return console::KC_EMPTY;
        
        if (bp_level == BP_CUSTOM)
        {
            if (msg != TString()) { under_lock_print(msg + TString("\n")); }
            else {
                ::std::map<TString, TString> sort;
                for (auto react : debugging_reactions_to_keys)
                {
                    sort[react.first.name()] = react.second.name;
                }

                TBufer buf;
                buf << "\t- break point -\n";
                for (auto pss : sort)
                {
                    buf << "[" << pss.first << "] - " << pss.second << "   ";
                }
                buf << "\n";

                under_lock_print(buf.str());
            }
        }
        
        return WaitReactToKeyCodes(debugging_keys, debugging_reactions_to_keys);
    }


    // ----------------------------------------------------------------------------------- debug macroses

    namespace detail {

        void start_macro(
            TString const & prefix,
            bool new_lin                   /* = true    */,
            bool brk_lin                   /* = false   */,
            char const * nesting_error_msg /* = nullptr */)
        {
            if (nes_accum)
            {
                if (nesting_error_msg)
                    [[unlikely]] throw std::logic_error(nesting_error_msg);
                if (brk_lin)
                    output_bufer << "\n" << std::left << std::setw(sib::console::tab_pos(1)) << prefix;
            }
            else
            {
                output_bufer.str({});
                output_bufer.clear();
                output_bufer << std::left << std::setw(sib::console::tab_width(0)) << prefix;

                if (new_lin)
                    output_bufer << std::left << std::setw(sib::console::tab_width(1)) << ++lin_accum;
            }
            ++nes_accum;
        }

        void to_drop_bufer()
        {
            under_lock_print(output_bufer.str());
            output_bufer.str({});
            output_bufer.clear();
        }

        void stop_macro(bool & stop_flag, TString const & msg /* = {} */)
        {
            if (stop_flag)
            {
                output_bufer << msg;
                to_drop_bufer();
                auto key = SetBreakPoint(BP_CUSTOM, "\n[Enter] - continue   [Esc] - abort   [F5] - ignore such");
                stop_flag = (key != sib::console::KC_F5);
            }
        }


        void finish_macro(TBreakPointLevel bp_level)
        {
            --nes_accum;
            if (!nes_accum)
            {
                output_bufer << '\n';
                to_drop_bufer();
                SetBreakPoint(bp_level);
            }
        }

        void new_begin()
        {
            ++beg_accum;
            lin_accum = 0;
        }

    } // namespace detail

    thread_local unsigned const & BEG_ACCUM = detail::beg_accum;
    thread_local unsigned const & LIN_ACCUM = detail::lin_accum;
    thread_local unsigned const & NES_ACCUM = detail::nes_accum;

} // namespace debug 
} // namespace sib
