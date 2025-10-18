#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <string>
#include <type_traits>
#include <functional>

#include "sib_type_info.h"
#include "sib_type_traits.h"
#include "sib_console.h"
#include "sib_string.h"

namespace sib {
namespace debug {

    namespace {
        using suppress_message_about_unused_included_header_sstream = std::basic_stringbuf<char>;
        using suppress_message_about_unused_included_header_sib_type_info = TTypeInfo<void>;
    }

    extern bool const & is_initialized;

    bool Init();

    #ifdef SIB_DEBUG_OUT_STREAM
        inline thread_local auto& outstream = SIB_DEBUG_OUT_STREAM;
    #else
        inline thread_local auto& outstream = ::sib::console::outstream;
    #endif // SIB_DEBUG_STREAM_CUSTOM
    
    using TOutStream = ::std::remove_reference_t<decltype(outstream)>;
    using OutStrmCh = typename TOutStream::char_type;
    using OutStrmTr = typename TOutStream::traits_type;

    #define SIB_DEGUG_LITERAL(txt) SIB_MAKE_LITERAL(::sib::debug::OutStrmCh, txt)

    using TBufer  = ::sib::promiscuous_stringstream <OutStrmCh, OutStrmTr>;
    using TString = ::sib::promiscuous_string       <OutStrmCh, OutStrmTr>;


    
    // Преобразование символов в строку, для вывода в outstream
    //   - управляющие символы выводятся как \<ESC> (<ESC> — символ, обозначающий Escape sequences)
    //   - символы в диапазоне OutStrmCh выводятся как есть
    //   - остальные выводятся как \i<NUM> (<NUM> — числовое значение по основанию 10)
    template <Char Ch>
    inline TString bufer_char_to_str(Ch ch)
    {
        switch (ch)
        {
            case '\0': return "\\0" ; // Null character
            case '\a': return "\\a" ; // Alert (bell)
            case '\b': return "\\b" ; // Backspace
            case '\t': return "\\t" ; // Horizontal tab
            case '\n': return "\\n" ; // New line
            case '\v': return "\\v" ; // Vertical tab
            case '\f': return "\\f" ; // Form feed
            case '\r': return "\\r" ; // Carriage return
            case  27 : return "\\e" ; // Escape
            case '\\': return "\\\\"; // Backslash
            case '\"': return "\\\""; // Double quote
        }

        auto sch = static_cast<OutStrmCh>(ch);
        if ((static_cast<Ch>(sch) == ch) and (isprint(ch)))
        {
            return sch;
        }
                   
        return "\\i" + ::std::to_string(static_cast<unsigned>(ch));
    }



    inline size_t CONTAINER_DISCLOSURE_LENGTH = 16;

    template <typename T>
    inline TString disclosure(T const & val)
    {
        {
            if constexpr                                                    ( ::std::is_same_v<T, bool>           ) {

                if constexpr (requires { TBufer() << ::std::boolalpha; })
                    { return (TBufer() << ::std::boolalpha << val).str(); }
                else
                    { return val ? "true" : "false";                    }

            } else if constexpr                                             (      is_char_v<T>                 ) {

                return (TBufer() << '\'' << bufer_char_to_str(val) << '\'').str();

            } else if constexpr                                             (      is_container_v<T>            ) {

                if constexpr (is_like_string_v<T>) {

                    if (std::size(val) == 0)
                    {
                        return "\"\"";
                    }
                    else
                    {
                        TBufer data;
                        data << "\"";
                        auto begin = ::std::begin(val);
                        auto end   = ::std::end  (val);
                        size_t counter = 1;
                        for (auto it = begin; it != end; ++it, ++counter)
                        {
                            if (counter > CONTAINER_DISCLOSURE_LENGTH)
                            {
                                data << "...";
                                return data.str();
                            }
                            data << bufer_char_to_str(*it);
                        }
                        data << "\"";
                        return data.str();
                    }

                } else {

                    if (std::size(val) == 0)
                    {
                        return "{}";
                    }
                    else
                    {
                        TBufer data;
                        data << "{ ";
                        auto begin = ::std::begin(val);
                        auto end   = ::std::end(val);
                        auto it    = begin;
                        size_t counter = 1;
                        data << disclosure(*it);
                        for (++it; it != end; ++it, ++counter)
                        {
                            if (counter > CONTAINER_DISCLOSURE_LENGTH)
                            {
                                data << "...";
                                return data.str();
                            }
                            data << ", " << disclosure(*it);
                        }
                        data << " }";
                        return data.str();
                    }

                }

            } else if constexpr                                             (      is_basic_string_v<T>         ) {

                as_basic_string_t<T> const & bs = val;
                return disclosure(bs);

            } else if constexpr                                             (      is_like_function_v<T>        ) {

                return "function";

            } else if constexpr                                             (      is_like_pointer_v<T>         ) {

                TBufer data;

                if constexpr (is_castable_from_to_v<T, void const * const>)
                {
                    auto ptr = static_cast<void const * const>(val);
                    if (ptr == nullptr) return disclosure(nullptr);
                    data << '[' << ptr << ']';
                }
                else
                {
                    data << "[???]";
                }

                if constexpr (is_dereferenceable_v<T>)
                {
                    using Content = base_of_indirect_type<T>;

                    if constexpr (is_char_v<Content> and ::std::incrementable<T>)
                    {
                        data << " \"";
                        size_t counter = 0;
                        for (Content* it = val; *it != '\0'; ++it, ++counter)
                        {
                            if (counter > CONTAINER_DISCLOSURE_LENGTH)
                            {
                                data << "...";
                                return data.str();
                            }
                            data << bufer_char(*it);
                        }
                        data << "\"";
                    }
                    else
                    {
                        TString str;
                        try { str = disclosure(*val); }
                        catch (...) { str = "!!!"; }
                        data << " { " << str << " }";
                    }
                
                }
                return data.str();

            } else {

                if constexpr (requires (T v) { TBufer{} << v; })
                    { return (TBufer() << val).str(); }
                else
                    { return "???";                   }

            }
        }
    };

    void under_lock_print(TString const& str);
    
    
    
// ----------------------------------------------------------------------------------- debug exceptions
    
    class EDebug : public ::std::runtime_error
    {
        using ::std::runtime_error::runtime_error;
    };
    
    class EDebugAbort : public EDebug
    {
        using EDebug::EDebug;
    };



// ----------------------------------------------------------------------------------- debug tests

    enum class TTestState { NotInitialized = 0, NotCompleted, Completed };
    
    enum class TTestLogType { message = 0, warning, error };
    static constexpr char const * test_log_type_name[] = { "Message", "Warning", "Error" };

    struct TTestLogRec
    {
        TTestLogType type;
        size_t       beg_num, lin_num;
        TString      description;
        
        TString united_message() const;
    };
    
    using TTestLog = ::std::vector<TTestLogRec>;
    
    // !!!
    // CUR_LOG is a required name for the sib_unit_test macros to work correctly.
    // Use the DEF_TEST macro to describe the function signature.
    using TTestFunc = ::std::function<int(TTestLog& /*CUR_LOG*/)>;

    template <typename F>
    concept TestFunc = requires(F f) { TTestFunc(f); };
    
    struct TTest
    {
    public:
        template <TestFunc F>
        TTest(F&& func) : _test(std::forward<F>(func)) {};

        TTestState const & state() const;
        TTestLog   const & log  () const;
        TTestFunc  const & test ()const;

        void run();
    private:
        TTestState _state{ TTestState::NotInitialized };
        TTestLog   _log{};
        TTestFunc  _test;

        void write_to_log(TTestLogType type, size_t beg_num, size_t lin_num, TString&& str);
        
        void message(size_t beg_num, size_t lin_num, TString&& str);
        void warning(size_t beg_num, size_t lin_num, TString&& str);
        void error  (size_t beg_num, size_t lin_num, TString&& str);
    };
    
    inline ::std::map<TString, TTest> Tests {};

    void RunAllTest();

    TString ReportText();



// ----------------------------------------------------------------------------------- debugging step by step

    enum TBreakPointLevel { BP_ALL = 0, BP_END, BP_BEGIN, BP_CUSTOM, BP_NONE };
    
    inline TBreakPointLevel current_break_level = BP_CUSTOM;
    
    inline console::TKeyCodeReactions debugging_reactions_to_keys{};
    
    console::TKeyCode SetBreakPoint(TBreakPointLevel bp_level = BP_CUSTOM, TString msg = {});
    


// ----------------------------------------------------------------------------------- debug macroses

    #define DEF_TEST(func_name) int func_name([[maybe_unused]]sib::debug::TTestLog& CUR_LOG)

    inline thread_local bool STOP_FLAG_ASSERTION_ERROR = true;
    inline thread_local bool STOP_FLAG_ASSERTION_FAIL = false;

    extern thread_local unsigned const & BEG_ACCUM;
    extern thread_local unsigned const & LIN_ACCUM;
    extern thread_local unsigned const & NES_ACCUM;

    namespace detail {

        struct error_tag
        {
            constexpr operator bool() { return false; };
        };

        template <typename T>
        error_tag to_bool(T const& val) { return {}; }

        template <typename T>
            requires std::is_convertible_v<T, bool>
        bool to_bool(T const& val) { return val; }

        extern thread_local TBufer output_bufer;

        void start_macro(
            TString const& prefix,
            bool new_lin                  = true   ,
            bool brk_lin                  = false  ,
            char const* nesting_error_msg = nullptr);
    
        void to_drop_bufer();
        
        void stop_macro(bool & stop_flag, TString const& msg = {});

        void finish_macro(TBreakPointLevel bp_level);
    
        void new_begin();

    } // namespace detail

    #define BP                                                                                          \
        ::sib::debug::  SetBreakPoint(sib::debug::BP_CUSTOM)                                            \

    #define BEG                                                                                         \
        ::sib::debug::detail::start_macro("b", false, false, "BEG is nested in another sib::debug macro."); \
        ::sib::debug::detail::new_begin();                                                              \
        ::sib::debug::detail::output_bufer                                                              \
            << "---------------------------------------------------------------------------------------------- " \
            << ::sib::debug::BEG_ACCUM;                                                                 \
        ::sib::debug::detail::finish_macro(sib::debug::BP_BEGIN);                                       \

    #define END                                                                                         \
        ::sib::debug::detail::start_macro("e", false, false, "AND is nested in another sib::debug macro."); \
        ::sib::debug::detail::finish_macro(sib::debug::BP_END)                                          \

    #define MSG(...)                                                                                    \
        ::sib::debug::detail::start_macro("m", false);                                                  \
        ::sib::debug::detail::output_bufer << ::sib::debug::TString(__VA_ARGS__);                       \
        ::sib::debug::detail::finish_macro(sib::debug::BP_ALL)                                          \

    #define EXE(...)                                                                                    \
        ::sib::debug::detail::start_macro("x");                                                         \
        ::sib::debug::detail::output_bufer << #__VA_ARGS__;                                             \
        ::sib::debug::detail::finish_macro(sib::debug::BP_ALL);                                         \
        __VA_ARGS__                                                                                     \

    #define TYP(...)                                                                                    \
        ::sib::debug::detail::start_macro("p");                                                         \
        if (not ::sib::debug::NES_ACCUM)                                                                \
            ::sib::debug::detail::output_bufer << #__VA_ARGS__;                                         \
        ::sib::debug::detail::output_bufer << " -> " << ::sib::type_name<__VA_ARGS__>();                \
        ::sib::debug::detail::finish_macro(sib::debug::BP_ALL)                                          \

    #define DEF(type, inst, ...)                                                                        \
        ::sib::debug::detail::start_macro("d");                                                         \
        EXE(type inst __VA_ARGS__);                                                                     \
        TYP(decltype(inst));                                                                            \
        ::sib::debug::detail::finish_macro(sib::debug::BP_ALL)                                          \

    #define ASS(...)                                                                                    \
        ::sib::debug::detail::start_macro("a", true, true);                                             \
        if constexpr (std::same_as<decltype(::sib::debug::detail::to_bool(__VA_ARGS__)), bool>)         \
        {                                                                                               \
            if (::sib::debug::detail::to_bool(__VA_ARGS__))                                             \
            {                                                                                           \
                ::sib::debug::detail::output_bufer << "[pass] ASSERT(" << #__VA_ARGS__ << ")";          \
            }                                                                                           \
            else                                                                                        \
            {                                                                                           \
                ::sib::debug::detail::output_bufer << "[FAIL] ASSERT(" << #__VA_ARGS__ << ")";          \
                CUR_LOG.emplace_back(                                                                   \
                    ::sib::debug::TTestLogType::error,                                                  \
                    ::sib::debug::BEG_ACCUM,                                                            \
                    ::sib::debug::LIN_ACCUM,                                                            \
                    "Assertion fail"                                                                    \
                );                                                                                      \
                ::sib::debug::detail::stop_macro(                                                       \
                    ::sib::debug::STOP_FLAG_ASSERTION_FAIL,                                             \
                    "\n    - assertion fail -"                                                          \
                );                                                                                      \
            }                                                                                           \
        }                                                                                               \
        else                                                                                            \
        {                                                                                               \
            ::sib::debug::detail::output_bufer                                                          \
                << "[ERROR] ASSERT(" << #__VA_ARGS__ << ") "                                            \
                << "Assertion statement is not convertible to bool";                                    \
            CUR_LOG.emplace_back(                                                                       \
                ::sib::debug::TTestLogType::error,                                                      \
                ::sib::debug::BEG_ACCUM,                                                                \
                ::sib::debug::LIN_ACCUM,                                                                \
                "Assertion error (statement is not convertible to bool)"                                \
            );                                                                                          \
            ::sib::debug::detail::stop_macro(                                                           \
                ::sib::debug::STOP_FLAG_ASSERTION_ERROR,                                                \
                "\n    - assertion error -"                                                             \
            );                                                                                          \
        }                                                                                               \
        ::sib::debug::detail::finish_macro(sib::debug::BP_ALL)                                          \

    #define TIS(type, ...) ASS(std::is_same_v<type, __VA_ARGS__>)

    #define EIS(expr, ...) ASS(std::is_same_v<decltype(expr), __VA_ARGS__>)

    #define DEFA(type, inst, init, ...)                                                                 \
        ::sib::debug::detail::start_macro("d");                                                         \
        EXE(type inst init);                                                                            \
        TYP(decltype(inst));                                                                            \
        EIS(inst, __VA_ARGS__);                                                                         \
        ::sib::debug::detail::finish_macro(sib::debug::BP_ALL)                                          \
    
    #define PRN(...)                                                                                    \
        ::sib::debug::detail::start_macro("p");                                                         \
        ::sib::debug::detail::output_bufer                                                              \
            << #__VA_ARGS__                                                                             \
            << " = "  << ::sib::debug::disclosure(__VA_ARGS__)                                          \
            << " -> " << ::sib::type_name<decltype(__VA_ARGS__)>();                                     \
        ::sib::debug::detail::finish_macro(sib::debug::BP_ALL)                                          \

    #define PAS(inst, ...)                                                                              \
        ::sib::debug::detail::start_macro("p");                                                         \
        ::sib::debug::detail::output_bufer                                                              \
            << SIB_DEGUG_LITERAL(SIB_STR_STRINGISE(inst))                                               \
            << " ~ "  << ::sib::debug::disclosure(static_cast<__VA_ARGS__>(inst))                       \
            << " -> " << #__VA_ARGS__;                                                                  \
        ::sib::debug::detail::finish_macro(sib::debug::BP_ALL)                                          \

} // namespace debug
} // namespace sib
