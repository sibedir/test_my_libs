#pragma once

#include <iostream>
#include <cstdint>
#include <set>
#include <map>
#include <string>
#include <cctype>
#include <functional>

#ifdef _WIN32
    #include <conio.h>
    #include <chrono>
    #include <thread>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>
#endif

namespace sib {

    class TKeyCode : private std::string
    {
    private:
        using base = std::string;
    public:
        TKeyCode() = default;
        TKeyCode(char v0)                            noexcept : base{ v0 } {}
        TKeyCode(char v0, char v1)                   noexcept : base{ v0, v1 } {}
        TKeyCode(char v0, char v1, char v2)          noexcept : base{ v0, v1, v2 } {}
        TKeyCode(char v0, char v1, char v2, char v3) noexcept : base{ v0, v1, v2, v3 } {}

        using base::size;
        using base::operator[];

        TKeyCode& operator<<(char ch);
        bool operator==(TKeyCode const& other) const noexcept;
        bool operator< (TKeyCode const& other) const noexcept;
        std::string name() const;
    };

    [[nodiscard]] inline TKeyCode GetKey() {
        TKeyCode res;
        #ifdef _WIN32
            res << static_cast<char>(_getch());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            while (_kbhit()) res << static_cast<char>(_getch());
        #else
            char ch;
            struct termios oldt, newt;

            // Сохраняем текущие настройки терминала
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;

            // Включаем raw-режим (без канонического ввода и эха)
            newt.c_lflag &= ~(ICANON | ECHO);

            // Читаем первый байт (блокирующе)
            newt.c_cc[VMIN]  = 1;
            newt.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            if (read(STDIN_FILENO, &ch, 1) == 1) res << ch;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            // Читаем оставшиеся байты из буфера (без ожидания)
            newt.c_cc[VMIN] = 0;
            newt.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            while (read(STDIN_FILENO, &ch, 1) == 1) res << ch;

            // Возвращаем терминал в исходное состояние
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        #endif
        return res;
    }

    extern TKeyCode KC_ENTER;
    extern TKeyCode KC_ESC  ;
                    
    extern TKeyCode KC_F1 ;
    extern TKeyCode KC_F2 ;
    extern TKeyCode KC_F3 ;
    extern TKeyCode KC_F4 ;
    extern TKeyCode KC_F5 ;
    extern TKeyCode KC_F6 ;
    extern TKeyCode KC_F7 ;
    extern TKeyCode KC_F8 ;
    extern TKeyCode KC_F9 ;
    extern TKeyCode KC_F10;
    extern TKeyCode KC_F11;
    extern TKeyCode KC_F12;

    extern TKeyCode KC_NUM_INSERT   ;
    extern TKeyCode KC_NUM_DELETE   ;
    extern TKeyCode KC_NUM_HOME     ;
    extern TKeyCode KC_NUM_END      ;
    extern TKeyCode KC_NUM_PAGE_UP  ;
    extern TKeyCode KC_NUM_PAGE_DOWN;

    extern TKeyCode KC_INSERT   ;
    extern TKeyCode KC_DELETE   ;
    extern TKeyCode KC_HOME     ;
    extern TKeyCode KC_END      ;
    extern TKeyCode KC_PAGE_UP  ;
    extern TKeyCode KC_PAGE_DOWN;

    extern TKeyCode KC_NUM_LEFT ;
    extern TKeyCode KC_NUM_RIGHT;
    extern TKeyCode KC_NUM_UP   ;
    extern TKeyCode KC_NUM_DOWN ;

    extern TKeyCode KC_LEFT ;
    extern TKeyCode KC_RIGHT;
    extern TKeyCode KC_UP   ;
    extern TKeyCode KC_DOWN ;

    using TKeyCodeNames     = std::map<TKeyCode, std::string>;
    using TKeyCodeReactions = std::map<TKeyCode, std::function<void()>>;

    extern TKeyCodeNames     KeyCodeNames            ;
    extern TKeyCodeReactions DefaultKeyCodeReactions ;
    extern bool ConsoleLibDefaultInitialization      ;

    inline TKeyCode WaitKeyCodes(
        std::set<TKeyCode> const& codes,
        std::string        const& msg = "")
    {
        std::cout << msg;
        while (true) {
            auto kc = codes.find(GetKey());
            if (kc != codes.end()) return *kc;
        }
    }

    inline TKeyCode WaitAnyKey(
        std::string const& msg = "")
    {
        std::cout << msg;
        return GetKey();
    }

    inline TKeyCode WaitReactToKeyCodes(
        std::set<TKeyCode> const& codes,
        TKeyCodeReactions  const& reactions = DefaultKeyCodeReactions,
        std::string        const& msg = "")
    {
        auto kc = WaitKeyCodes(codes, msg);
        auto react = reactions.find(kc);
        if (react != reactions.end()) react->second();
        return kc;
    }

    inline TKeyCode WaitReactToAnyKey(
        TKeyCodeReactions const& reactions = DefaultKeyCodeReactions,
        std::string                const& msg = "")
    {
        auto kc = WaitAnyKey(msg);
        auto react = reactions.find(kc);
        if (react != reactions.end()) react->second();
        return kc;
    }


} // namespace sib
