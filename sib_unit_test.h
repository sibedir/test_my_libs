#pragma once

#include <utility>
#include <iostream>

#include "sib_support.h"

extern bool const is_initialized_unit_test_module;

// ----------------------------------------------------------------------------------- debug print

struct debug_endl_t {};

constexpr debug_endl_t debug_endl;

namespace {

    struct fake_outstream_type
    {
        template <typename T>
        void operator << (T const &) const { std::cout << "???"; }
    };

    static constexpr fake_outstream_type fake_outstream{};

    template <typename T = char>
    constexpr auto& outstream() {
        if      constexpr (requires (T const & v) { std:: cout << v; }) { return std::cout     ; }
        else if constexpr (requires (T const & v) { std::wcout << v; }) { return std::wcout    ; }
        else                                                            { return fake_outstream; }
    }

    template <sib::Char Ch>
    inline void _debug_print_char(Ch ch)
    {
        if constexpr (sib::is_any_of_v<Ch, char8_t, char16_t, char32_t>)
        {
            outstream() << '#' << static_cast<long>(ch);
        }
        else
        {
            switch (ch)
            {
                case '\0': outstream() << "\\0"; break;
                case '\a': outstream() << "\\a"; break;
                case '\b': outstream() << "\\b"; break;
                case '\t': outstream() << "\\t"; break;
                case '\n': outstream() << "\\n"; break;
                case '\v': outstream() << "\\v"; break;
                case '\f': outstream() << "\\f"; break;
                case '\r': outstream() << "\\r"; break;
                default:   outstream<Ch>() << ch;
            }
        }
    }

    template <typename T>
    inline void _debug_print(T const & val)
    {
        if      constexpr (std::is_same_v<T, debug_endl_t>)
        {
            outstream() << '\n';
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            outstream() << (val ? "true" : "false");
        }
        else if constexpr (sib::is_char_v<T>)
        {
            outstream() << "'";
            _debug_print_char(val);
            outstream() << "'";
        }
        else if constexpr (sib::is_container_v<T>)
        {
            if constexpr (sib::is_like_string_v<T>) {
                outstream() << "\"";
                auto begin = std::begin(val);
                auto end   = std::end  (val);
                for (auto it = begin; it != end; ++it)
                {
                    if (it - begin > 32) {
                        outstream() << "...";
                        break;
                    }
                    _debug_print_char(*it);
                }
                outstream() << "\"";
            }
            else
            {
                if (std::begin(val) == std::end(val))
                {
                    outstream() << "{}";
                    return;
                }

                outstream() << "{ ";
                auto end   = std::end  (val);
                auto it    = std::begin(val);
                _debug_print(*it);
                for (++it; it != end; ++it)
                {
                    outstream() << ", ";
                    _debug_print(*it);
                }
                outstream() << " }";
            }
        }
        else if constexpr (sib::is_like_function_v<T>)
        {
            outstream() << "function";
        }
        else if constexpr (sib::is_like_pointer_v<T>)
        {
            if (!val)
            {
                outstream() << nullptr;
                return;
            }

            if constexpr (std::is_convertible_v<T, void const*>)
            {
                outstream() << static_cast<void const*>(val);
            }
            else
            {
                outstream() << val;
            }

            if constexpr (sib::may_be_indirect_v<T>)
            {
                if constexpr (sib::is_char_v<sib::base_indirection_type<T>>)
                {
                    outstream() << " \"";
                    for (sib::base_indirection_type<T>* it = val; *it != '\0'; ++it)
                    {
                        if (it - val > 16)
                        {
                            outstream() << "...";
                            break;
                        }
                        _debug_print_char(*it);
                    }
                    outstream() << "\"";
                }
                else
                {
                    outstream() << " { ";
                    _debug_print(*val);
                    outstream() << " }";
                }
            }
        }
        else
        {
            outstream<decltype(val)>() << val;
        }
    }
}

template <typename T>
inline void debug_print(T const & first)
{
    _debug_print(first);
}

template <typename T, typename... Ts>
inline void debug_print(T const & first, Ts&& ... others)
{
    _debug_print(first);
     debug_print(std::forward<Ts>(others)...);
}



// ----------------------------------------------------------------------------------- debugging step by step

enum TBreakPointLevel { BP_ALL = 0, BP_END, BP_BEGIN, BP_CUSTOM, BP_NONE };

extern TBreakPointLevel current_break_level; // debin default value is BP_CUSTOM;

extern inline void SetBreakPoint(TBreakPointLevel bp_level = BP_ALL);

#define STR(...) #__VA_ARGS__

#define _ ,

static int BEG_COUNTR = 0;

#define BEG                                                     \
    std::cout << ++BEG_COUNTR << " ----------------------------------------------------------------------------------------------\n\n";\
    SetBreakPoint(BP_BEGIN)                                     \

#define DEF(type, inst, init)                                   \
    type inst init;                                             \
    do {                                                        \
        constexpr auto __typ__ = sib::type_name<decltype(inst)>();  \
        std::string inst_str { STR(init) };                     \
        std::cout                                               \
            <<   "d   " << STR(type)                            \
            <<      " " << #inst                                \
            <<      " " << inst_str                             \
            << "  ->  " << __typ__                              \
            << "\n";                                            \
    } while(0);                                                 \
    SetBreakPoint(BP_ALL)                                       \


#define DEFA(type, inst, init, type_assert)                     \
    DEF(type, inst, init);                                      \
    static_assert(                                              \
        std::is_same_v<decltype(inst), type_assert>,            \
        "Incorrect assumption in DEFA about the inferred type ( decltype("#inst") is not "#type_assert" )." \
    )                                                           \

#define PRN(instance)                                           \
    do {                                                        \
        constexpr auto __typ__ = sib::type_name<decltype(instance)>();\
        std::cout                                               \
            << "p       |" << STR(instance)                     \
            <<     "  =  ";                                     \
        debug_print(instance);                                  \
        std::cout                                               \
            <<      " -> " << __typ__                           \
            << '\n';                                            \
    } while(0);                                                 \
    SetBreakPoint(BP_ALL)                                       \

#define PRNAS(instance, type_as)                                \
    do {                                                        \
        constexpr auto __typ__    = sib::type_name<decltype(instance)>();   \
        constexpr auto __typ_as__ = sib::type_name<type_as           >();   \
        std::cout                                               \
            << "p       |" << STR(instance)                     \
            <<        " {" << __typ__    << "}"                 \
            <<     " -> {" << __typ_as__ << "}"                 \
            <<     "  =  ";                                     \
        debug_print(static_cast<type_as>(instance));            \
        std::cout                                               \
            << '\n';                                            \
    } while(0);                                                 \
    SetBreakPoint(BP_ALL)                                       \

#define EXE(...)                                                \
    std::cout << "e   " << #__VA_ARGS__ << ";\n";               \
    __VA_ARGS__;                                                \
    SetBreakPoint(BP_ALL)                                       \

#define BP SetBreakPoint(BP_CUSTOM)                             \

#define END                                                     \
    std::cout << "\n";                                          \
    SetBreakPoint(BP_END)                                       \
