﻿#include <random>
#include <chrono>
#include <conio.h>

#include "sib_support.h"

namespace sib {

    // ----------------------------------------------------------------------------------- rand

    ::std::random_device __rd;
    ::std::mt19937 __gen(__rd());
    ::std::uniform_real_distribution<double> __dis(0, 1);

    /* random in [0, 1) */ inline double rand() { return __dis(__gen); }
    /* random in [0, X) */ inline double rand(double X) { return rand() * X; }



    // ----------------------------------------------------------------------------------- convertion

    inline std::wstring string_to_wstring(std::string const& str)
    {
        return std::wstring(str.begin(), str.end());
    }

    inline std::wstring string_to_wstring()
    {
        return std::wstring();
    }

    inline std::string wstring_to_string(std::wstring const& str)
    {
        return std::string(str.begin(), str.end());
    }

    inline std::string wstring_to_string()
    {
        return std::string();
    }



    // ----------------------------------------------------------------------------------- console

    Tconsole_reactions_to_keys default_console_reactions_to_keys{};

    [[nodiscard]] inline TKeyCode GetKeyCode()
    {
        TKeyCode res = _getch();
        if (res == KC_COMM_1 or res == KC_COMM_2)
            res = res * 256 + _getch();
        return res;
    }

    inline TKeyCode WaitKeyCodes(std::set<TKeyCode> const & codes, std::string const & msg)
    {
        std::cout << msg;
        while (true) {
            auto kc = codes.find(GetKeyCode());
            if (kc != codes.end()) return *kc;
        }
    }

    inline TKeyCode WaitAnyKey(std::string const & msg)
    {
        std::cout << msg;
        return GetKeyCode();
    }

    inline TKeyCode WaitReactToKeyCodes(
        std::set<TKeyCode>         const & codes,
        Tconsole_reactions_to_keys const & console_reactions_to_keys,
        std::string                const & msg)
    {
        auto kc = console_reactions_to_keys.find(WaitKeyCodes(codes, msg));
        if (kc != console_reactions_to_keys.end()) kc->second();
        return kc->first;
    }
    
    inline TKeyCode WaitReactToAnyKey(
        Tconsole_reactions_to_keys const & console_reactions_to_keys,
        std::string                const & msg)
    {
        auto kc = console_reactions_to_keys.find(WaitAnyKey(msg));
        if (kc != console_reactions_to_keys.end()) kc->second();
        return kc->first;
    }

}  // namespace sib
