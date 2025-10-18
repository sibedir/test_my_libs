#pragma once

#include <iostream>
//#include <sstream>
#include <set>
#include <map>
#include <string>
#include <cctype>
#include <functional>
#include "sib_type_traits.h"
#include "sib_string.h"

namespace sib {
namespace console {

    extern bool const & is_initialized;

    bool Init();

    #ifdef SIB_OUT_STREAM
        inline thread_local auto& outstream = SIB_OUT_STREAM;
    #else
        inline thread_local auto& outstream = ::std::cout;
    #endif // SIB_DEBUG_STREAM_CUSTOM

    using TOutStream = ::std::remove_reference_t<decltype(outstream)>;
    using OutStrmCh = typename TOutStream::char_type;
    using OutStrmTr = typename TOutStream::traits_type;

    using TBufer  = ::sib::promiscuous_stringstream<OutStrmCh, OutStrmTr>;
    using TString = ::sib::promiscuous_string      <OutStrmCh, OutStrmTr>;



    // ----------------------------------------------------------------------------------- tabs

    extern thread_local int TAB_DEF_WIDTH;
    extern thread_local ::std::vector<int> TAB_WIDTH;

    int tab_width(size_t idx);

    int tab_pos(size_t idx);



    // ----------------------------------------------------------------------------------- TKeyCode

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

    inline TKeyCode KC_EMPTY = {};

    inline TKeyCode KC_ENTER            ;
    inline TKeyCode KC_ESC              ;
    inline TKeyCode KC_TAB              ;
    inline TKeyCode KC_SPACE            ;
    inline TKeyCode KC_BACKSPACE        ;

    inline TKeyCode KC_CTRL_ENTER       ;
    inline TKeyCode KC_CTRL_TAB         ;
    inline TKeyCode KC_CTRL_BACKSPACE   ;

    inline TKeyCode KC_F1               ;
    inline TKeyCode KC_F2               ;
    inline TKeyCode KC_F3               ;
    inline TKeyCode KC_F4               ;
    inline TKeyCode KC_F5               ;
    inline TKeyCode KC_F6               ;
    inline TKeyCode KC_F7               ;
    inline TKeyCode KC_F8               ;
    inline TKeyCode KC_F9               ;
    inline TKeyCode KC_F10              ;
    inline TKeyCode KC_F11              ;
    inline TKeyCode KC_F12              ;

    inline TKeyCode KC_NUM_INSERT       ;
    inline TKeyCode KC_NUM_DELETE       ;
    inline TKeyCode KC_NUM_HOME         ;
    inline TKeyCode KC_NUM_END          ;
    inline TKeyCode KC_NUM_PAGE_UP      ;
    inline TKeyCode KC_NUM_PAGE_DOWN    ;

    inline TKeyCode KC_INSERT           ;
    inline TKeyCode KC_DELETE           ;
    inline TKeyCode KC_HOME             ;
    inline TKeyCode KC_END              ;
    inline TKeyCode KC_PAGE_UP          ;
    inline TKeyCode KC_PAGE_DOWN        ;

    inline TKeyCode KC_NUM_LEFT         ;
    inline TKeyCode KC_NUM_RIGHT        ;
    inline TKeyCode KC_NUM_UP           ;
    inline TKeyCode KC_NUM_DOWN         ;

    inline TKeyCode KC_LEFT             ;
    inline TKeyCode KC_RIGHT            ;
    inline TKeyCode KC_UP               ;
    inline TKeyCode KC_DOWN             ;

    using TKeyCodeNames     = ::std::map<TKeyCode, ::std::string>;

    namespace detail {
        using TActionBase = ::std::pair<::std::string, ::std::function<void()>>;
    }

    struct TAction //: detail::TActionBase
    {
        using name_type = detail::TActionBase:: first_type;
        using func_type = detail::TActionBase::second_type;

        name_type name;
        func_type func;

        //name_type const & name() const { return this->first ; }
        //name_type       & name()       { return this->first ; }
        //func_type const & func() const { return this->second; }
        //func_type       & func()       { return this->second; }
    };
    
    using TKeyCodeReactions = ::std::map<TKeyCode, TAction>;

    inline TKeyCodeNames     KeyCodeNames            {};
    inline TKeyCodeReactions DefaultKeyCodeReactions {};
    

    // ----------------------------------------------------------------------------------- console functions

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
    [[nodiscard]] TKeyCode GetKey();

    TKeyCode WaitKeyCodes(
        ::std::set<TKeyCode> const& codes,
        TString              const& msg = {}
    );

    TKeyCode WaitAnyKey(
        TString const& msg = {}
    );

    TKeyCode WaitReactToKeyCodes(
        ::std::set<TKeyCode> const& codes,
        TKeyCodeReactions    const& reactions = DefaultKeyCodeReactions,
        TString              const& msg       = {}
    );

    TKeyCode WaitReactToAnyKey(
        TKeyCodeReactions const& reactions = DefaultKeyCodeReactions,
        TString           const& msg       = {}
    );

} // namespace console
} // namespace sib
