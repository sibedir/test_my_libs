#pragma once

#include <iostream>

#include "sib_support.h"

extern bool const is_initialized_unit_test_module;

// ----------------------------------------------------------------------------------- debug print

#define OUTSTREAM std::cout

template <typename T>
inline void debug_print(T const& val) {
    OUTSTREAM << val;
}

template <std::same_as<bool> T>
inline void debug_print(T const & val) {
    OUTSTREAM << (val ? "true" : "false");
}

template <sib::Char Ch>
inline void debug_print(Ch const & ch) {
    if (ch == 0) { OUTSTREAM << "#0";             }
    else         { OUTSTREAM << "'" << ch << "'"; }
}

template <sib::Container Cont> requires (not sib::is_basic_string_v<Cont>)
inline void debug_print(Cont const & cont) {
    if (std::begin(cont) == std::end(cont)) {
        OUTSTREAM << "{}";
        return;
    }

    OUTSTREAM << "{ ";
    auto it = std::begin(cont);
    debug_print(*it);
    for (++it; it != std::end(cont); ++it) {
        OUTSTREAM << ", ";
        debug_print(*it);
    }
    OUTSTREAM << " }";
}

template <sib::Basic_string Str>
inline void debug_print(Str const & str) {
    OUTSTREAM << "\"" << str << "\"";
}

template <sib::Like_pointer P>
    requires (
        not sib::is_like_function_v<P>
        and
        not std::is_array_v<P>
        and
        not sib::is_container_v<P>
    )
inline void debug_print(P const & ptr) {
    if (!ptr) {
        OUTSTREAM << nullptr;
        return;
    }

    OUTSTREAM << static_cast<void const*>(ptr);
    if constexpr (sib::may_be_indirect_v<P>) {
        OUTSTREAM << " { ";
        debug_print(*ptr);
        OUTSTREAM << " }";
    }
}

template <sib::Like_function F>
inline void debug_print(F const &) {
    OUTSTREAM << "function " << typeid(F).name();
}



// ----------------------------------------------------------------------------------- debugging step by step

enum TBreakPointLevel { BP_ALL = 0, BP_END, BP_BEGIN, BP_CUSTOM, BP_NONE };

extern TBreakPointLevel current_break_level; // debin default value is BP_CUSTOM;

extern inline void SetBreakPoint(TBreakPointLevel bp_level = BP_ALL);

#define STR(...) #__VA_ARGS__

#define _ ,

static int BEG_COUNTR = 0;

#define BEG                                                     \
    OUTSTREAM << ++BEG_COUNTR << " ----------------------------------------------------------------------------------------------\n\n";\
    SetBreakPoint(BP_BEGIN)                                     \

#define DEF(type, inst, init)                                   \
    type inst init;                                             \
    do {                                                        \
        std::string __init__{STR(init)};                        \
        OUTSTREAM                                               \
            <<   "d   " << STR(type)                            \
            <<      " " << #inst                                \
            <<      " " << __init__                             \
            << "  ->  "                                         \
            << sib::TTypeInfo<decltype(inst)>().full_name()     \
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
        auto __typ__ = sib::TTypeInfo<decltype(instance)>().full_name();\
        OUTSTREAM                                               \
            << "p       |" << STR(instance)                     \
            <<     "  =  ";                                     \
        debug_print(instance);                                  \
        OUTSTREAM                                               \
            <<      " -> " << __typ__                           \
            << std::endl;                                       \
    } while(0);                                                 \
    SetBreakPoint(BP_ALL)                                       \

#define PRNAS(instance, type)                                   \
    do {                                                        \
        auto __typ__ = sib::TTypeInfo<decltype(instance)>().full_name();\
        auto __typ_as__ = sib::TTypeInfo<type>().full_name();   \
        OUTSTREAM                                               \
            << "p       |" << STR(instance)                     \
            <<        " {" << __typ__    << "}"                 \
            <<     " -> {" << __typ_as__ << "}"                 \
            <<     "  =  ";                                     \
        debug_print(static_cast<type>(instance));               \
        OUTSTREAM                                               \
            << std::endl;                                       \
    } while(0);                                                 \
    SetBreakPoint(BP_ALL)                                       \

#define EXE(line)                                               \
    OUTSTREAM << "e   " << STR(line) << ";\n";                  \
    line;                                                       \
    SetBreakPoint(BP_ALL)                                       \

#define BP SetBreakPoint(BP_CUSTOM)                             \

#define END                                                     \
    OUTSTREAM << "\n";                                          \
    SetBreakPoint(BP_END)                                       \
