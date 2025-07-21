#pragma once

#ifdef SIB_DEBUG

    #include <iostream>
    #include <sstream>
    #include <utility>
    #include <string>
    #include <mutex>
    
    using namespace std::string_literals;
    
    #include "sib_type_info.h"
    #include "sib_support.h"
   
    namespace sib {
    namespace debug {
    
        extern bool const is_initialized_unit_test_module;

        inline std::mutex mtx{};
    
        #ifdef SIB_DEBUG_OUT_STREAM
            inline auto& outstream = SIB_DEBUG_OUT_STREAM;
        #else
            inline auto& outstream = std::cout;
        #endif // SIB_DEBUG_STREAM_CUSTOM
    
        using TOutStream = std::remove_reference_t<decltype(outstream)>;
        using OutStrmCh = typename TOutStream::char_type;
        using OutStrmTr = typename TOutStream::traits_type;
        using TBufer = std::basic_ostringstream<OutStrmCh, OutStrmTr, std::allocator<OutStrmCh>>;
    
        class string : public decltype(std::declval<TBufer>().str())
        {
        public:
            using basic_string = decltype(std::declval<TBufer>().str());
    
            string() = default;

            string(basic_string const & str) : basic_string(          str ) {}
            string(basic_string      && str) : basic_string(std::move(str)) {}
    
            template <LikeString Str>
            string(Str const & str) : basic_string(str.begin(), str.end()) {}
    
            template <Char Ch>
            string(Ch const * const ptr) : string(std::basic_string<Ch, std::char_traits<Ch>, std::allocator<Ch>>(ptr)) {}
    
            string(value_type ch) : basic_string{ ch } {}
        };
    
        inline void under_lock_print(string const& str)
        {
            std::lock_guard lock(mtx);
            outstream << str;
            outstream.flush();
        }
    
        // Преобразование символов в строку, для вывода в outstream
        //   - управляющие символы выводятся как \<ESC> (<ESC> — символ, обозначающий Escape sequences)
        //   - символы в диапазоне OutStrmCh выводятся как есть
        //   - остальные выводятся как \i<NUM> (<NUM> — числовое значение по основанию 10)
        template <Char Ch>
        string bufer_char(Ch ch)
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
            if (static_cast<Ch>(sch) == ch)
            {
                return sch;
            }
                       
            return "\\i" + std::to_string(static_cast<unsigned>(ch));
        }
    
    
    
        inline int DISCLOSURE_STRING_LENGTH = 16;
    
        template <typename T>
        string disclosure(T const & val)
        {
            TBufer data1;
            {
                if constexpr                                                    ( std::is_same_v<T, bool>           ) {
    
                    if constexpr (requires { TBufer() << std::boolalpha; })
                        { return (TBufer() << std::boolalpha << val).str(); }
                    else
                        { return val ? "true" : "false";                    }
    
                } else if constexpr                                             ( sib::is_char_v<T>                 ) {
    
                    return (TBufer() << '\'' << bufer_char(val) << '\'').str();
    
                } else if constexpr                                             ( sib::is_container_v<T>            ) {
    
                    if constexpr (sib::is_like_string_v<T>) {
    
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
                            for (auto it = begin; it != end; ++it)
                            {
                                if (it - begin > DISCLOSURE_STRING_LENGTH)
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
                            data << disclosure(*it);
                            for (++it; it != end; ++it)
                            {
                                if (it - begin > DISCLOSURE_STRING_LENGTH)
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
    
                } else if constexpr                                             ( sib::is_as_basic_string_v<T>      ) {

                    sib::as_basic_string_t<T> const & bs = val;
                    return disclosure(bs);
    
                } else if constexpr                                             ( sib::is_like_function_v<T>        ) {
    
                    return "function";
    
                } else if constexpr                                             ( sib::is_like_pointer_v<T>         ) {
    
                    TBufer data;
                    data << '[' << static_cast<void const * const>(val) << ']';
                    if constexpr (sib::may_be_indirect_v<T>)
                    {
                        if (static_cast<bool>(val))
                        {
                            using Content = sib::base_indirection_type<T>;
    
                            if constexpr (sib::is_char_v<Content>)
                            {
                                data << " \"";
                                for (Content* it = val; *it != '\0'; ++it)
                                {
                                    if (it - val > DISCLOSURE_STRING_LENGTH)
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
                                data << " { " << disclosure(*val) << " }";
                            }
                        }
                    }
                    return data.str();
    
                } else {
    
                    if constexpr (requires (T const & v) { TBufer{} << v; })
                        { return (TBufer() << val).str(); }
                    else
                        { return "???";                   }
    
                }
            }
        };

        template <typename... Args>
        string msg(Args&&... args)
        {
            TBufer buf;
            (buf << ... << std::forward<Args>(args));
            return buf.str();
        }
    
    
    
    // ----------------------------------------------------------------------------------- debugging step by step

        enum TBreakPointLevel { BP_ALL = 0, BP_END, BP_BEGIN, BP_CUSTOM, BP_NONE };
        
        inline TBreakPointLevel current_break_level = BP_CUSTOM;
        
        inline sib::Tconsole_reactions_to_keys debugging_reactions_to_keys{};
        
        bool const is_initialized_unit_test_module = []()
            {
                debugging_reactions_to_keys[sib::KC_ESC  ] = []() { abort();                         };
                debugging_reactions_to_keys[sib::KC_ENTER] = []() {                                  };
                debugging_reactions_to_keys[sib::KC_F5   ] = []() { current_break_level = BP_CUSTOM; };
                debugging_reactions_to_keys[sib::KC_F10  ] = []() { current_break_level = BP_BEGIN;  };
                debugging_reactions_to_keys[sib::KC_F11  ] = []() { current_break_level = BP_END;    };
                debugging_reactions_to_keys[sib::KC_F12  ] = []() { current_break_level = BP_ALL;    };
                return true;
            }
        ();
        
        inline void SetBreakPoint(TBreakPointLevel bp_level = BP_CUSTOM, string msg = "")
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
        
            if (bp_level == BP_CUSTOM)
            {
                if (msg != "") { under_lock_print(msg + "\n"); }
                else { under_lock_print("       - break point -       [Enter] - continue   [Esc] - abort\n"); }
            }
        
            sib::WaitReactToKeyCodes(debugging_keys, debugging_reactions_to_keys);
        }
        
        static int BEG_COUNTR = 0;

    } // namespace debug 
    } // namespace sib

    #define _STR(...) #__VA_ARGS__

    #define BEG                                                     \
        do {                                                        \
            sib::debug::TBufer buf;                                 \
            buf << ++sib::debug::BEG_COUNTR                         \
                << " ----------------------------------------------------------------------------------------------\n"; \
            sib::debug::under_lock_print(buf.str());                \
        } while(0);                                                 \
        sib::debug::SetBreakPoint(sib::debug::BP_BEGIN)             \
    
    #define DEF(type, inst, init)                                   \
        type inst init;                                             \
        do {                                                        \
            auto __typ__ = sib::type_name<decltype(inst)>();        \
            sib::debug::TBufer buf;                                 \
            buf <<   "d   " << sib::debug::string(_STR(type))        \
                <<      " " << sib::debug::string(_STR(inst))        \
                <<      " " << sib::debug::string(_STR(init))        \
                << "  ->  " << __typ__                              \
                << "\n";                                            \
            sib::debug::under_lock_print(buf.str());                \
        } while(0);                                                 \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)               \
    
    #define DEFA(type, inst, init, type_assert)                     \
        DEF(type, inst, init);                                      \
        static_assert(                                              \
            std::is_same_v<decltype(inst), type_assert>,            \
            "Incorrect assumption in DEFA about the inferred type ( decltype("#inst") is not "#type_assert" )." \
        )                                                           \
    
    #define PRN(inst)                                               \
        do {                                                        \
            auto __typ__ = sib::type_name<decltype(inst)>();        \
            sib::debug::TBufer buf;                                 \
            buf << "p       |" << sib::debug::string(_STR(inst))     \
                <<     "  =  " << sib::debug::disclosure(inst)      \
                <<      " -> " << __typ__                           \
                << "\n";                                            \
            sib::debug::under_lock_print(buf.str());                \
        } while(0);                                                 \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)               \
    
    #define PRNAS(inst, type_as)                                    \
        do {                                                        \
            auto __typ__ = sib::type_name<decltype(inst)>();        \
            sib::debug::TBufer buf;                                 \
            buf << "p       |" << sib::debug::string(_STR(inst))     \
                <<        " {" << __typ__  << "}"                   \
                <<     "  ~  " << sib::debug::disclosure(static_cast<type_as>(inst)) \
                <<     " -> {" << sib::debug::string(_STR(type_as)) << "}" \
                << "\n";                                            \
            sib::debug::under_lock_print(buf.str());                \
        } while(0);                                                 \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)               \

    #define EXE(...)                                                \
        do {                                                        \
            sib::debug::TBufer buf;                                 \
            buf << "e   " << #__VA_ARGS__ << ";\n";                 \
            sib::debug::under_lock_print(buf.str());                \
        } while(0);                                                 \
        __VA_ARGS__;                                                \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)               \

    #define MSG(...)                                                \
        do {                                                        \
            sib::debug::TBufer buf;                                 \
            buf << "m   " << sib::debug::msg(__VA_ARGS__) << "\n";  \
            sib::debug::under_lock_print(buf.str());                \
        } while(0);                                                 \
        sib::debug::SetBreakPoint(sib::debug::BP_ALL)               \


    #define BP sib::debug::SetBreakPoint(sib::debug::BP_CUSTOM)     \

    #define END                                                     \
        sib::debug::under_lock_print('\n');                         \
        sib::debug::SetBreakPoint(sib::debug::BP_END)               \

    #define ASSERT(...)                                             \
        do {                                                        \
            sib::debug::TBufer buf;                                 \
            buf << "a       |(" << #__VA_ARGS__ << ") = ";          \
            auto statement = (__VA_ARGS__);                         \
            if constexpr (!std::is_same_v<decltype(statement), bool>) {\
                buf << "NOT BOOL!\n";                               \
                sib::debug::under_lock_print(buf.str());            \
                sib::debug::SetBreakPoint(                          \
                    sib::debug::BP_CUSTOM,                          \
                    "     - assertion error -     [Enter] - continue   [Esc] - abort"); \
            } else if (statement) {                                 \
                buf << "true\n";                                    \
                sib::debug::under_lock_print(buf.str());            \
                sib::debug::SetBreakPoint(sib::debug::BP_ALL);      \
            } else {                                                \
                buf << "FALSE!!!\n";                                \
                sib::debug::under_lock_print(buf.str());            \
                sib::debug::SetBreakPoint(                          \
                    sib::debug::BP_CUSTOM,                          \
                    "     - assertion fail -      [Enter] - continue   [Esc] - abort"); \
            }                                                       \
        } while(0);                                                 \

#else

    #define BEG
    
    #define DEF(type, inst, init) type inst init
    
    #define DEFA(type, inst, init, type_assert) DEF(type, inst, init)
    
    #define PRN(inst)
    
    #define PRNAS(inst, type_as)

    #define EXE(...) __VA_ARGS__

    #define MSG(...)

    #define BP

    #define END

    #define ASSERT(...)

#endif // SIB_DEBUG                                          
