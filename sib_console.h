#pragma once

#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <string>
//#include <cctype>
#include <functional>
#include "sib_type_traits.h"

#if defined(_WIN32)
    #include <windows.h>
    #include "conio.h"
    #include <synchapi.h>
#else
    #include <cerrno>
    #include <unistd.h>
    #include <termios.h>
    #include "sib_support.h"
#endif

namespace sib {
namespace console {

    extern bool const & is_initialized_console_unit;

    bool Init();

    #ifdef SIB_OUT_STREAM
        inline auto& outstream = SIB_OUT_STREAM;
    #else
        inline auto& outstream = ::std::cout;
    #endif // SIB_DEBUG_STREAM_CUSTOM

    using TOutStream = ::std::remove_reference_t<decltype(outstream)>;
    using OutStrmCh = typename TOutStream::char_type;
    using OutStrmTr = typename TOutStream::traits_type;

    class string;

    class TBufer : ::std::basic_stringstream<OutStrmCh, OutStrmTr, ::std::allocator<OutStrmCh>>
    {
    private:

        using base_type = ::std::basic_stringstream<OutStrmCh, OutStrmTr, ::std::allocator<OutStrmCh>>;

    public:

        using base_type::str;

        template <typename T>
            requires(requires(T&& t, base_type bs) { bs << ::std::forward<T>(t); })
        TBufer& operator<< (T&& arg)
        {
            static_cast<base_type&>(*this) << ::std::forward<T>(arg);
            return *this;
        }

        template <typename _Ch, typename _Tr, typename _Al>
            requires(not ::std::is_same_v<::std::basic_string<_Ch, _Tr, _Al>, string>)
        inline TBufer& operator<< (std::basic_string<_Ch, _Tr, _Al> const & str)
        {
            for (_Ch ch : str) static_cast<base_type&>(*this) << static_cast<OutStrmCh>(ch);
            return *this;
        }

        template <typename _Ch, typename _Tr>
            requires(not (     ::std::is_same_v<_Ch, OutStrmCh>
                         and ::std::is_same_v<_Tr, OutStrmTr> ))
        inline TBufer& operator<< (std::basic_string_view<_Ch, _Tr> const & str)
        {
            for (_Ch ch : str) static_cast<base_type&>(*this) << static_cast<OutStrmCh>(ch);
            return *this;
        }

    };

    class string : public ::std::basic_string<OutStrmCh, OutStrmTr, ::std::allocator<OutStrmCh>>
    {
    private:
        template <typename... Args>
        string init(Args&&... args)
        {
            TBufer buf;
            (buf << ... << ::std::forward<Args>(args));
            return buf.str();
        }
    public:
        using basic_string = ::std::basic_string<OutStrmCh, OutStrmTr, ::std::allocator<OutStrmCh>>;

        string() = default;

        string(basic_string const & str) : basic_string(          str ) {}
        string(basic_string      && str) : basic_string(std::move(str)) {}

        template <typename _Ch, typename _Tr, typename _Al>
        string(std::basic_string<_Ch, _Tr, _Al> const & str) : basic_string(str) {}

        template <::sib::LikeString Str>
        string(Str const & str) : basic_string(str.begin(), str.end()) {}

        template <::sib::Char Ch>
        string(Ch const * ptr) : string(std::basic_string<Ch, ::std::char_traits<Ch>, ::std::allocator<Ch>>(ptr)) {}

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

        return "\\i" + ::std::to_string(static_cast<unsigned>(ch));
    }



    class TKeyCode : private ::std::string
    {
    private:
        using base = ::std::string;
        using byte = ::std::byte;
    public:
        using base::value_type;

        TKeyCode() = default;
        TKeyCode(byte v0)                            noexcept : base{ char(v0) } {}
        TKeyCode(byte v0, byte v1)                   noexcept : base{ char(v0), char(v1) } {}
        TKeyCode(byte v0, byte v1, byte v2)          noexcept : base{ char(v0), char(v1), char(v2) } {}
        TKeyCode(byte v0, byte v1, byte v2, byte v3) noexcept : base{ char(v0), char(v1), char(v2), char(v3) } {}
        TKeyCode(std::initializer_list<value_type> ilist) : base(ilist) {}

        using base::size;
        using base::operator[];
        using base::insert;
        using base::begin;
        using base::end;

        TKeyCode& operator<<(char ch);
        bool operator==(TKeyCode const& other) const noexcept;
        bool operator< (TKeyCode const& other) const noexcept;
        ::std::string name() const;
    };
    
    /*
    EN:
        Purpose:
            Reads simultaneous multiple key presses or a single
            key press.
        Returns: TKeyCode
            Sequential set of console key codes (char).
        Notes:
            - Blocking call: from the first key press until the
              console buffer is empty.
            - For POSIX version, key presses are considered
              simultaneous if they occur within ~0.1 sec interval.
              Waits for the first press + 0.1 sec and reads the entire
              console buffer in chunks until empty.
            - Suitable for reading arrow keys, function keys, etc.
            - !!! Intercepts signal keys such as Ctrl+C, Esc, etc.
            - Only captures keys not intercepted by the window
              manager. For example, F11 on Ubuntu doesn't reach the
              input stream as it toggles console window fullscreen
              mode. Same applies to Alt+Enter on Windows.
    RU:
        Назначение:
            Считывание одновременного нажатия нескольких клавиш или
            отдельного нажатия одной клавиши.
        Результат: TKeyCode
            Последовательный набор кодов клавиш (char) консоли.
        Примечания:
            - Вызов блокирующий: от первого нажатия до опустошения
              буфера консоли.
            - Для POSIX версии одновременным считается нажатие в
              интервале ~0,1 сек. Ждёт первого нажатия + 0,1 сек и
              считывает порциями весь буфер консоли до опустошения.
            - Подходит для чтения стрелок, функциональных клавиш и т.п.
            - !!! Перехватывает сигнальные клавиши Ctrl+C, Esc и т.д.
            - Перехватывает только клавиши не перехваченные оконным
              менеджером. Например F11 на Ubuntu не попадает в поток
              ввода, а переводит окно консоли в полноэкранный режим.
              То же самое для Alt+Enter для Windows.
    */
    [[nodiscard]] inline TKeyCode GetKey() {
        TKeyCode res;

        #if defined(_WIN32)

            res << static_cast<char>(_getch());
            while (_kbhit()) res << static_cast<char>(_getch());

        #elif defined(_POSIX_VERSION)

            char buf[16];
            ssize_t bytesRead;

            // save terminal mode
            termios oldt;
            if (tcgetattr(STDIN_FILENO, &oldt) == -1) return res;
            termios curt = oldt;
            SIB_SCOPE_GUARD( (void)tcsetattr(STDIN_FILENO, TCSANOW, &oldt); );

            // first read buf
            {
                // set raw & block mode
                curt.c_lflag &= ~(ISIG | ICANON | ECHO);
                curt.c_cc[VMIN] = 1;
                curt.c_cc[VTIME] = 0;
                if (tcsetattr(STDIN_FILENO, TCSANOW, &curt) == -1) return res;
            
                bytesRead = ::read(STDIN_FILENO, buf, sizeof(buf));
                usleep(100000); // 0.1 sec

                // set not block mode
                curt.c_cc[VMIN] = 0;
                if (tcsetattr(STDIN_FILENO, TCSANOW, &curt) == -1) return res;
            }

            for(;; bytesRead = ::read(STDIN_FILENO, buf, sizeof(buf)))
            {
                if (bytesRead > 0)
                {
                    res.insert(res.end(), buf, buf + bytesRead);
                    continue;
                }
                if ((bytesRead == 0) or (errno != EINTR)) break;
            }

        #else

            static_assert(false, "unknown platform");

        #endif
        return res;
    }

    inline TKeyCode KC_ENTER;
    inline TKeyCode KC_ESC  ;

    inline TKeyCode KC_F1 ;
    inline TKeyCode KC_F2 ;
    inline TKeyCode KC_F3 ;
    inline TKeyCode KC_F4 ;
    inline TKeyCode KC_F5 ;
    inline TKeyCode KC_F6 ;
    inline TKeyCode KC_F7 ;
    inline TKeyCode KC_F8 ;
    inline TKeyCode KC_F9 ;
    inline TKeyCode KC_F10;
    inline TKeyCode KC_F11;
    inline TKeyCode KC_F12;

    inline TKeyCode KC_NUM_INSERT   ;
    inline TKeyCode KC_NUM_DELETE   ;
    inline TKeyCode KC_NUM_HOME     ;
    inline TKeyCode KC_NUM_END      ;
    inline TKeyCode KC_NUM_PAGE_UP  ;
    inline TKeyCode KC_NUM_PAGE_DOWN;

    inline TKeyCode KC_INSERT   ;
    inline TKeyCode KC_DELETE   ;
    inline TKeyCode KC_HOME     ;
    inline TKeyCode KC_END      ;
    inline TKeyCode KC_PAGE_UP  ;
    inline TKeyCode KC_PAGE_DOWN;

    inline TKeyCode KC_NUM_LEFT ;
    inline TKeyCode KC_NUM_RIGHT;
    inline TKeyCode KC_NUM_UP   ;
    inline TKeyCode KC_NUM_DOWN ;

    inline TKeyCode KC_LEFT ;
    inline TKeyCode KC_RIGHT;
    inline TKeyCode KC_UP   ;
    inline TKeyCode KC_DOWN ;

    using TKeyCodeNames     = ::std::map<TKeyCode, ::std::string>;
    using TKeyCodeReactions = ::std::map<TKeyCode, ::std::function<void()>>;

    inline TKeyCodeNames     KeyCodeNames            {};
    inline TKeyCodeReactions DefaultKeyCodeReactions {};

    inline TKeyCode WaitKeyCodes(
        ::std::set<TKeyCode> const& codes,
        string             const& msg = {})
    {
        outstream << msg;
        outstream.flush();
        while (true) {
            auto kc = codes.find(GetKey());
            if (kc != codes.end()) return *kc;
        }
    }

    inline TKeyCode WaitAnyKey(
        string const& msg = {})
    {
        outstream << msg;
        outstream.flush();
        return GetKey();
    }

    inline TKeyCode WaitReactToKeyCodes(
        ::std::set<TKeyCode> const& codes,
        TKeyCodeReactions  const& reactions = DefaultKeyCodeReactions,
        string             const& msg = {})
    {
        auto kc = WaitKeyCodes(codes, msg);
        auto react = reactions.find(kc);
        if (react != reactions.end()) react->second();
        return kc;
    }

    inline TKeyCode WaitReactToAnyKey(
        TKeyCodeReactions const& reactions = DefaultKeyCodeReactions,
        string            const& msg = {})
    {
        auto kc = WaitAnyKey(msg);
        auto react = reactions.find(kc);
        if (react != reactions.end()) react->second();
        return kc;
    }


} // namespace console
} // namespace sib
