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

namespace sib {
namespace debug {

    namespace { using suppress_message_about_unused_included_header = TTypeInfo<void>; }

    extern bool const & is_initialized;

    bool Init();

    #ifdef SIB_DEBUG_OUT_STREAM
        inline auto& outstream = SIB_DEBUG_OUT_STREAM;
    #else
        inline auto& outstream = std::cout;
    #endif // SIB_DEBUG_STREAM_CUSTOM
    
    using TOutStream = std::remove_reference_t<decltype(outstream)>;
    using OutStrmCh = typename TOutStream::char_type;
    using OutStrmTr = typename TOutStream::traits_type;

    #define SIB_DEGUG_LITERAL(txt) SIB_MAKE_LITERAL(sib::debug::OutStrmCh, txt)

    class string;

    class TBufer : std::basic_stringstream<OutStrmCh, OutStrmTr, std::allocator<OutStrmCh>>
    {
    private:
    
        using base_type = std::basic_stringstream<OutStrmCh, OutStrmTr, std::allocator<OutStrmCh>>;

    public:
    
        using base_type::str;
    
        template <typename T>
            requires(requires(T&& t, base_type bs) { bs << std::forward<T>(t); })
        TBufer& operator<< (T&& arg)
        {
            static_cast<base_type&>(*this) << std::forward<T>(arg);
            return *this;
        }

        template <typename _Ch, typename _Tr, typename _Al>
            requires(not std::is_same_v<std::basic_string<_Ch, _Tr, _Al>, string>)
        inline TBufer& operator<< (std::basic_string<_Ch, _Tr, _Al> const & str)
        {
            for (_Ch ch : str) static_cast<base_type&>(*this) << static_cast<sib::debug::OutStrmCh>(ch);
            return *this;
        }
        
        template <typename _Ch, typename _Tr>
            requires(not (     std::is_same_v<_Ch, OutStrmCh>
                           and std::is_same_v<_Tr, OutStrmTr> ))
        inline TBufer& operator<< (std::basic_string_view<_Ch, _Tr> const & str)
        {
            for (_Ch ch : str) static_cast<base_type&>(*this) << static_cast<sib::debug::OutStrmCh>(ch);
            return *this;
        }

    };

    class string : public std::basic_string<OutStrmCh, OutStrmTr, std::allocator<OutStrmCh>>
    {
    private:
        template <typename... Args>
        string init(Args&&... args)
        {
            TBufer buf;
            (buf << ... << std::forward<Args>(args));
            return buf.str();
        }
    public:
        using basic_string = std::basic_string<OutStrmCh, OutStrmTr, std::allocator<OutStrmCh>>;

        string() = default;

        string(basic_string const & str) : basic_string(          str ) {}
        string(basic_string      && str) : basic_string(std::move(str)) {}

        template <typename _Ch, typename _Tr, typename _Al>
        string(std::basic_string<_Ch, _Tr, _Al> const & str) : basic_string(str) {}

        template <LikeString Str>
        string(Str const & str) : basic_string(str.begin(), str.end()) {}

        template <Char Ch>
        string(Ch const * ptr) : string(std::basic_string<Ch, std::char_traits<Ch>, std::allocator<Ch>>(ptr)) {}

        string(value_type ch) : basic_string{ ch } {}

        template <typename... Args>
        string (Args&&... args) : basic_string(init(std::forward<Args>(args) ...)) {}
    };
    
    // Преобразование символов в строку, для вывода в outstream
    //   - управляющие символы выводятся как \<ESC> (<ESC> — символ, обозначающий Escape sequences)
    //   - символы в диапазоне OutStrmCh выводятся как есть
    //   - остальные выводятся как \i<NUM> (<NUM> — числовое значение по основанию 10)
    template <Char Ch>
    inline string bufer_char(Ch ch)
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
                   
        return "\\i" + std::to_string(static_cast<unsigned>(ch));
    }



    inline size_t DISCLOSURE_STRING_LENGTH = 16;

    template <typename T>
    inline string disclosure(T const & val)
    {
        {
            if constexpr                                                    ( std::is_same_v<T, bool>           ) {

                if constexpr (requires { TBufer() << std::boolalpha; })
                    { return (TBufer() << std::boolalpha << val).str(); }
                else
                    { return val ? "true" : "false";                    }

            } else if constexpr                                             (      is_char_v<T>                 ) {

                return (TBufer() << '\'' << bufer_char(val) << '\'').str();

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
                        auto begin = std::begin(val);
                        auto end   = std::end  (val);
                        size_t counter = 1;
                        for (auto it = begin; it != end; ++it, ++counter)
                        {
                            if (counter > DISCLOSURE_STRING_LENGTH)
                            {
                                data << "...";
                                return data.str();
                            }
                            data << bufer_char(*it);
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
                        auto begin = std::begin(val);
                        auto end   = std::end(val);
                        auto it    = begin;
                        size_t counter = 1;
                        data << disclosure(*it);
                        for (++it; it != end; ++it, ++counter)
                        {
                            if (counter > DISCLOSURE_STRING_LENGTH)
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

                    if constexpr (is_char_v<Content> and std::incrementable<T>)
                    {
                        data << " \"";
                        size_t counter = 0;
                        for (Content* it = val; *it != '\0'; ++it, ++counter)
                        {
                            if (counter > DISCLOSURE_STRING_LENGTH)
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
                        string str;
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

    void under_lock_print(string const& str);
    
    
    
// ----------------------------------------------------------------------------------- debug exceptions
    
    class EDebug : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
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
        TTestLogType typ;
        string       str;
        
        string united_message() const;
    };
    
    using TTestLog = std::vector<TTestLogRec>;
    
    // !!!
    // CUR_LOG is a required name for the sib_unit_test macros to work correctly.
    // Use the DEF_TEST macro to describe the function signature.
    using TTestFunc = std::function<int(TTestLog& /*CUR_LOG*/)>;

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

        void write_to_log(TTestLogType st, string&& str);
        
        void message(string&& str);
        void warning(string&& str);
        void error  (string&& str);
    };
    
    inline std::map<string, TTest> Tests {};

    void RunAllTest();



// ----------------------------------------------------------------------------------- debugging step by step

    enum TBreakPointLevel { BP_ALL = 0, BP_END, BP_BEGIN, BP_CUSTOM, BP_NONE };
    
    inline TBreakPointLevel current_break_level = BP_CUSTOM;
    
    inline TKeyCodeReactions debugging_reactions_to_keys{};
    
    void SetBreakPoint(TBreakPointLevel bp_level = BP_CUSTOM, string msg = string{});
    
    inline int BEG_COUNTR = 0;



// ----------------------------------------------------------------------------------- debug macroses

    #define DEF_TEST(func_name) int func_name([[maybe_unused]]sib::debug::TTestLog& CUR_LOG)

    inline constexpr bool _to_bool_(...) { return false; }
    inline constexpr bool _to_bool_(bool val) { return val; }

    #define ASSERT(...)                                                     \
        do {                                                                \
            auto __VA_ARGS_STR__ = SIB_DEGUG_LITERAL(#__VA_ARGS__);         \
            sib::debug::TBufer __buf__;                                     \
            __buf__ << "a       |" << __VA_ARGS_STR__;                      \
            auto statement = (__VA_ARGS__);                                 \
            if constexpr (!std::is_convertible_v<decltype(statement), bool>)\
            {                                                               \
                __buf__ << " - NOT convertible to bool\n";                  \
                sib::debug::under_lock_print(__buf__.str());                \
                auto st = sib::debug::TTestLogType::error;                  \
                sib::debug::string str                                      \
                (                                                           \
                    "Assertion error (statement is not convertible to bool): ASSERT(", __VA_ARGS_STR__, ")" \
                );                                                          \
                CUR_LOG.emplace_back(st, str);                              \
                sib::debug::SetBreakPoint(                                  \
                    sib::debug::BP_CUSTOM,                                  \
                    "     - assertion error -      [Enter] - continue   [Esc] - abort" \
                );                                                          \
            } else if (sib::debug::_to_bool_(statement)) {                  \
                __buf__ << " == true\n";                                    \
                sib::debug::under_lock_print(__buf__.str());                \
                sib::debug::SetBreakPoint(sib::debug::BP_ALL);              \
            } else {                                                        \
                __buf__ << " == FALSE\n";                                   \
                sib::debug::under_lock_print(__buf__.str());                \
                auto st = sib::debug::TTestLogType::error;                  \
                sib::debug::string str                                      \
                (                                                           \
                    "Assertion fail: ASSERT(", __VA_ARGS_STR__, ")"         \
                );                                                          \
                CUR_LOG.emplace_back(st, str);                              \
                sib::debug::SetBreakPoint(                                  \
                    sib::debug::BP_CUSTOM,                                  \
                    "     - assertion fail -      [Enter] - continue   [Esc] - abort" \
                );                                                          \
            }                                                               \
        } while(0);                                                         \

    #define TYPE_ASSERT(expr, type_assert)                                  \
        do {                                                                \
            auto __expr_str__ = SIB_DEGUG_LITERAL(#expr);                   \
            using type = decltype(expr);                                    \
            auto type_name = sib::type_name<type>();                        \
            sib::debug::TBufer __buf__;                                     \
            __buf__ << "a       |" << __expr_str__;                         \
            if constexpr (std::is_same_v<type, type_assert>) {              \
                __buf__ << " --> " << #type_assert << "\n";                 \
                sib::debug::under_lock_print(__buf__.str());                \
                sib::debug::SetBreakPoint(sib::debug::BP_ALL);              \
            } else {                                                        \
                __buf__ << " -NOT-> " << #type_assert << "\n";              \
                sib::debug::under_lock_print(__buf__.str());                \
                auto st = sib::debug::TTestLogType::error;                  \
                sib::debug::string str                                      \
                (                                                           \
                    "Type assertion fail: TYPE_ASSERT(", __expr_str__, ", ", #type_assert, ")" \
                );                                                          \
                CUR_LOG.emplace_back(st, str);                              \
                sib::debug::SetBreakPoint(                                  \
                    sib::debug::BP_CUSTOM,                                  \
                    "     - assertion fail -     [Enter] - continue   [Esc] - abort"); \
            }                                                               \
        } while(0);                                                         \

    #define BEG                                                             \
        do {                                                                \
            sib::debug::TBufer __buf__;                                     \
            __buf__                                                         \
                << "---------------------------------------------------------------------------------------------- " \
                << ++sib::debug::BEG_COUNTR                                 \
                << "\n";                                                    \
            sib::debug::under_lock_print(__buf__.str());                    \
        } while(0);                                                         \
        sib::debug::SetBreakPoint(sib::debug::BP_BEGIN)                     \
    
    #define DEF(type, inst, init)                                           \
        type inst init;                                                     \
        do {                                                                \
            auto __type_str__ = SIB_DEGUG_LITERAL(#type);                   \
            auto __inst_str__ = SIB_DEGUG_LITERAL(#inst);                   \
            auto __init_str__ = SIB_DEGUG_LITERAL(#init);                   \
            auto __typ__ = sib::type_name<decltype(inst)>();                \
            sib::debug::TBufer __buf__;                                     \
            __buf__                                                         \
                <<   "d   " << __type_str__                                 \
                <<      " " << __inst_str__                                 \
                <<      " " << __init_str__                                 \
                << "  ->  " << __typ__                                      \
                << "\n";                                                    \
            sib::debug::under_lock_print(__buf__.str());                    \
        } while(0);                                                         \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)                       \
    
    #define DEFA(type, inst, init, type_assert)                             \
        type inst init;                                                     \
        do {                                                                \
            auto __type_str__        = SIB_DEGUG_LITERAL(#type       );     \
            auto __inst_str__        = SIB_DEGUG_LITERAL(#inst       );     \
            auto __init_str__        = SIB_DEGUG_LITERAL(#init       );     \
            auto __type_assert_str__ = SIB_DEGUG_LITERAL(#type_assert);     \
            using __T__ = decltype(inst);                                   \
            using __TA__ = type_assert;                                     \
            auto __TN__ = sib::type_name<__T__>();                          \
            sib::debug::TBufer __buf__;                                     \
            __buf__                                                         \
                <<   "d   " << __type_str__                                 \
                <<      " " << __inst_str__                                 \
                <<      " " << __init_str__                                 \
                << "  ->  " << __TN__;                                      \
            if constexpr (std::is_same_v<__T__, __TA__>) {                  \
                __buf__ <<   " --> "  << __type_assert_str__ << "\n";       \
                sib::debug::under_lock_print(__buf__.str());                \
                sib::debug::SetBreakPoint(sib::debug::BP_ALL);              \
            } else {                                                        \
                __buf__ << " -NOT-> " << __type_assert_str__ << "\n";       \
                sib::debug::under_lock_print(__buf__.str());                \
                auto st = sib::debug::TTestLogType::error;                  \
                sib::debug::string str                                      \
                (                                                           \
                    "Type assertion fail: DEFA(", __type_str__, __inst_str__, __init_str__, __type_assert_str__, ")" \
                );                                                          \
                CUR_LOG.emplace_back(st, str);                              \
                sib::debug::SetBreakPoint(                                  \
                    sib::debug::BP_CUSTOM,                                  \
                    "     - assertion fail -     [Enter] - continue   [Esc] - abort"); \
            }                                                               \
        } while(0)                                                          \
    
    #define PRN(...)                                                        \
        do {                                                                \
            auto __VA_ARGS_STR__ = SIB_DEGUG_LITERAL(#__VA_ARGS__);         \
            auto const & __ref__ = __VA_ARGS__;                             \
            sib::debug::TBufer __buf__;                                     \
            __buf__                                                         \
                << "p       |" << __VA_ARGS_STR__                           \
                <<     "  =  " << sib::debug::disclosure(__ref__)           \
                <<      " -> " << sib::static_type_name<decltype(__VA_ARGS__)>() \
                << "\n";                                                    \
            sib::debug::under_lock_print(__buf__.str());                    \
        } while(0);                                                         \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)                       \
    
    #define PRNAS(inst, type_as)                                            \
        do {                                                                \
            auto __inst__    = SIB_DEGUG_LITERAL(#inst   );                 \
            auto __type_as__ = SIB_DEGUG_LITERAL(#type_as);                 \
            sib::debug::TBufer __buf__;                                     \
            __buf__                                                         \
                << "p       |" << __inst__                                  \
                <<     "  ~  " << sib::debug::disclosure(static_cast<type_as>(inst)) \
                <<      " ~> " << __type_as__                               \
                << "\n";                                                    \
            sib::debug::under_lock_print(__buf__.str());                    \
        } while(0);                                                         \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)                       \

    #define TYP(...)                                                        \
        do {                                                                \
            auto __VA_ARGS_STR__ = SIB_DEGUG_LITERAL(#__VA_ARGS__);         \
            sib::debug::TBufer __buf__;                                     \
            __buf__                                                         \
                << "p       |" << __VA_ARGS_STR__                           \
                <<      " -> " << sib::type_name<__VA_ARGS__>()             \
                << "\n";                                                    \
            sib::debug::under_lock_print(__buf__.str());                    \
        } while(0);                                                         \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)                       \
    
    #define EXE(...)                                                        \
        do {                                                                \
            auto __VA_ARGS_STR__ = SIB_DEGUG_LITERAL(#__VA_ARGS__);         \
            sib::debug::TBufer __buf__;                                     \
            __buf__ << "x   " << __VA_ARGS_STR__ << ";\n";                  \
            sib::debug::under_lock_print(__buf__.str());                    \
        } while(0);                                                         \
        __VA_ARGS__;                                                        \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)                       \
    
    #define MSG(...)                                                        \
        do {                                                                \
            auto __VA_ARGS_STR__ = SIB_DEGUG_LITERAL(#__VA_ARGS__);         \
            sib::debug::TBufer __buf__;                                     \
            __buf__ << "m   " << __VA_ARGS_STR__ << "\n";                   \
            sib::debug::under_lock_print(__buf__.str());                    \
        } while(0);                                                         \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)                       \

    #define BP sib::debug::SetBreakPoint(sib::debug::BP_CUSTOM)             \
    
    #define END                                                             \
        sib::debug::under_lock_print("\n");                                 \
        sib::debug::SetBreakPoint(sib::debug::BP_END)                       \

} // namespace debug 
} // namespace sib
