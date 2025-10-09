#include "sib_console.h"

namespace sib {
namespace console {

    // ----------------------------------------------------------------------------------- TKeyCode

    TKeyCode& TKeyCode::operator<<(char ch)
    {
        base::push_back(ch); return *this;
    }

    bool TKeyCode::operator==(TKeyCode const& other) const noexcept
    {
        return static_cast<base const &>(*this) == static_cast<base const &>(other);
    }

    bool TKeyCode::operator< (TKeyCode const& other) const noexcept
    {
        return static_cast<base const &>(*this) < static_cast<base const &>(other);
    }

    ::std::string TKeyCode::name() const
    {
        if (size() == 0) return "";

        auto it = KeyCodeNames.find(*this);
        if (it != KeyCodeNames.end()) return it->second;

        ::std::string res;
        res = ::std::to_string(static_cast<unsigned char>((*this)[0]));
        for (size_t i = 1; i < size(); ++i)
            res += ", " + ::std::to_string(static_cast<unsigned char>((*this)[i]));
        return res;
    }



    // ----------------------------------------------------------------------------------- console lib initialization

    static bool is_initialized_val = false;

    bool const & is_initialized_console_unit = is_initialized_val;

    bool Init()
    {
        if (is_initialized_val) return true;

        // default KeyCodes
        #ifdef _WIN32
    
            KC_ENTER          << char( 13);
            KC_ESC            << char( 27);
            KC_TAB            << char(  9);
            KC_SPACE          << char( 32);
            KC_BACKSPACE      << char(  8);

            KC_CTRL_ENTER     << char( 10);
            KC_CTRL_TAB       << char(  0) << char(148);
            KC_CTRL_BACKSPACE << char(127);

            KC_F1  << char(  0) << char( 59);
            KC_F2  << char(  0) << char( 60);
            KC_F3  << char(  0) << char( 61);
            KC_F4  << char(  0) << char( 62);
            KC_F5  << char(  0) << char( 63);
            KC_F6  << char(  0) << char( 64);
            KC_F7  << char(  0) << char( 65);
            KC_F8  << char(  0) << char( 66);
            KC_F9  << char(  0) << char( 67);
            KC_F10 << char(  0) << char( 68);
            KC_F11 << char(224) << char(133);
            KC_F12 << char(224) << char(134);
    
            KC_NUM_INSERT    << char(0) << char(82);
            KC_NUM_DELETE    << char(0) << char(83);
            KC_NUM_HOME      << char(0) << char(71);
            KC_NUM_END       << char(0) << char(79);
            KC_NUM_PAGE_UP   << char(0) << char(73);
            KC_NUM_PAGE_DOWN << char(0) << char(81);
    
            KC_INSERT    << char(224) << char(82);
            KC_DELETE    << char(224) << char(83);
            KC_HOME      << char(224) << char(71);
            KC_END       << char(224) << char(79);
            KC_PAGE_UP   << char(224) << char(73);
            KC_PAGE_DOWN << char(224) << char(81);
    
            KC_NUM_LEFT  << char(0) << char(75);
            KC_NUM_RIGHT << char(0) << char(77);
            KC_NUM_UP    << char(0) << char(72);
            KC_NUM_DOWN  << char(0) << char(80);
    
            KC_LEFT  << char(224) << char(75);
            KC_RIGHT << char(224) << char(77);
            KC_UP    << char(224) << char(72);
            KC_DOWN  << char(224) << char(80);
    
        #elif defined(_POSIX_VERSION)
    
            KC_ENTER << '\n';
            KC_ESC   << '\e';
    
            KC_F1  << '\e' << char(79) << char(80);
            KC_F2  << '\e' << char(79) << char(81);
            KC_F3  << '\e' << char(79) << char(82);
            KC_F4  << '\e' << char(79) << char(83);
            KC_F5  << '\e' << char(91) << char(49) << char(53) << char(126);
            KC_F6  << '\e' << char(91) << char(49) << char(55) << char(126);
            KC_F7  << '\e' << char(91) << char(49) << char(56) << char(126);
            KC_F8  << '\e' << char(91) << char(49) << char(57) << char(126);
            KC_F9  << '\e' << char(91) << char(50) << char(48) << char(126);
            KC_F10 << '\e' << char(91) << char(50) << char(49) << char(126);
            KC_F11 << '\e' << char(91) << char(50) << char(51) << char(126);
            KC_F12 << '\e' << char(91) << char(50) << char(52) << char(126);
    
            KC_INSERT    << '\e' << char(91) << char(50) << char(126);
            KC_DELETE    << '\e' << char(91) << char(51) << char(126);
            KC_HOME      << '\e' << char(91) << char(72)             ;
            KC_END       << '\e' << char(91) << char(70)             ;
            KC_PAGE_UP   << '\e' << char(91) << char(53) << char(126);
            KC_PAGE_DOWN << '\e' << char(91) << char(54) << char(126);
    
            KC_NUM_INSERT    << '\e' << char(91) << char(50) << char(126);
            KC_NUM_DELETE    << '\e' << char(91) << char(51) << char(126);
            KC_NUM_HOME      << '\e' << char(91) << char(72)             ;
            KC_NUM_END       << '\e' << char(91) << char(76)             ;
            KC_NUM_PAGE_UP   << '\e' << char(91) << char(53) << char(126);
            KC_NUM_PAGE_DOWN << '\e' << char(91) << char(54) << char(126);

            KC_UP    << '\e' << char(91) << char(65);
            KC_DOWN  << '\e' << char(91) << char(66);
            KC_RIGHT << '\e' << char(91) << char(67);
            KC_LEFT  << '\e' << char(91) << char(68);

            KC_NUM_LEFT  << '\e' << char(91) << char(65);
            KC_NUM_RIGHT << '\e' << char(91) << char(66);
            KC_NUM_UP    << '\e' << char(91) << char(67);
            KC_NUM_DOWN  << '\e' << char(91) << char(68);

        #else

            #ifndef DISABLE_SIB_WARNINGS
            #ifndef DISABLE_WARNING_UNKNOWN_KEY_CODES
    
                #pragma message (                                                                       \
                    "Warning [SIB]: Unknown platform. KC_<> values ​​are not set. "                       \
                    "To suppress this warning, define the DISABLE_WARNING_UNKNOWN_KEY_CODES macros."    \
                )                                                                                       \

            #endif
            #endif

        #endif

        // default KeyCodeNames registration

        KeyCodeNames[KC_ENTER         ] = "ENTER"           ;
        KeyCodeNames[KC_ESC           ] = "ESC"             ;
        KeyCodeNames[KC_TAB           ] = "TAB"             ;
        KeyCodeNames[KC_SPACE         ] = "SPACE"           ;
        KeyCodeNames[KC_BACKSPACE     ] = "BACKSPACE"       ;

        KeyCodeNames[KC_CTRL_ENTER    ] = "Ctrl, ENTER"     ;
        KeyCodeNames[KC_CTRL_TAB      ] = "Ctrl, TAB"       ;
        KeyCodeNames[KC_CTRL_BACKSPACE] = "Ctrl, BACKSPACE" ;

        KeyCodeNames[KC_F1            ] = "F1"              ; 
        KeyCodeNames[KC_F2            ] = "F2"              ; 
        KeyCodeNames[KC_F3            ] = "F3"              ; 
        KeyCodeNames[KC_F4            ] = "F4"              ; 
        KeyCodeNames[KC_F5            ] = "F5"              ; 
        KeyCodeNames[KC_F6            ] = "F6"              ; 
        KeyCodeNames[KC_F7            ] = "F7"              ; 
        KeyCodeNames[KC_F8            ] = "F8"              ; 
        KeyCodeNames[KC_F9            ] = "F9"              ; 
        KeyCodeNames[KC_F10           ] = "F10"             ; 
        KeyCodeNames[KC_F11           ] = "F11"             ; 
        KeyCodeNames[KC_F12           ] = "F12"             ; 
                                      
        KeyCodeNames[KC_NUM_INSERT    ] = "NUM_INSERT"      ; 
        KeyCodeNames[KC_NUM_DELETE    ] = "NUM_DELETE"      ; 
        KeyCodeNames[KC_NUM_HOME      ] = "NUM_HOME"        ; 
        KeyCodeNames[KC_NUM_END       ] = "NUM_END"         ; 
        KeyCodeNames[KC_NUM_PAGE_UP   ] = "NUM_PAGE_UP"     ; 
        KeyCodeNames[KC_NUM_PAGE_DOWN ] = "NUM_PAGE_DOWN"   ; 
                                      
        KeyCodeNames[KC_INSERT        ] = "INSERT"          ; 
        KeyCodeNames[KC_DELETE        ] = "DELETE"          ; 
        KeyCodeNames[KC_HOME          ] = "HOME"            ; 
        KeyCodeNames[KC_END           ] = "END"             ; 
        KeyCodeNames[KC_PAGE_UP       ] = "PAGE_UP"         ; 
        KeyCodeNames[KC_PAGE_DOWN     ] = "PAGE_DOWN"       ; 
                                      
        KeyCodeNames[KC_NUM_LEFT      ] = "NUM_LEFT"        ; 
        KeyCodeNames[KC_NUM_RIGHT     ] = "NUM_RIGHT"       ; 
        KeyCodeNames[KC_NUM_UP        ] = "NUM_UP"          ; 
        KeyCodeNames[KC_NUM_DOWN      ] = "NUM_DOWN"        ; 
                                      
        KeyCodeNames[KC_LEFT          ] = "LEFT"            ; 
        KeyCodeNames[KC_RIGHT         ] = "RIGHT"           ; 
        KeyCodeNames[KC_UP            ] = "UP"              ; 
        KeyCodeNames[KC_DOWN          ] = "DOWN"            ; 

        // ASCII
        KeyCodeNames[{char( 33)}] = "!";
        KeyCodeNames[{char( 34)}] = "\"";
        KeyCodeNames[{char( 35)}] = "#";
        KeyCodeNames[{char( 36)}] = "$";
        KeyCodeNames[{char( 37)}] = "%";
        KeyCodeNames[{char( 38)}] = "&";
        KeyCodeNames[{char( 39)}] = "'";
        KeyCodeNames[{char( 40)}] = "(";
        KeyCodeNames[{char( 41)}] = ")";
        KeyCodeNames[{char( 42)}] = "*";
        KeyCodeNames[{char( 43)}] = "+";
        KeyCodeNames[{char( 44)}] = ",";
        KeyCodeNames[{char( 45)}] = "-";
        KeyCodeNames[{char( 46)}] = ".";
        KeyCodeNames[{char( 47)}] = "/";
        KeyCodeNames[{char( 48)}] = "0";
        KeyCodeNames[{char( 49)}] = "1";
        KeyCodeNames[{char( 50)}] = "2";
        KeyCodeNames[{char( 51)}] = "3";
        KeyCodeNames[{char( 52)}] = "4";
        KeyCodeNames[{char( 53)}] = "5";
        KeyCodeNames[{char( 54)}] = "6";
        KeyCodeNames[{char( 55)}] = "7";
        KeyCodeNames[{char( 56)}] = "8";
        KeyCodeNames[{char( 57)}] = "9";
        KeyCodeNames[{char( 58)}] = ":";
        KeyCodeNames[{char( 59)}] = ";";
        KeyCodeNames[{char( 60)}] = "<";
        KeyCodeNames[{char( 61)}] = "=";
        KeyCodeNames[{char( 62)}] = ">";
        KeyCodeNames[{char( 63)}] = "?";
        KeyCodeNames[{char( 64)}] = "@";
        KeyCodeNames[{char( 65)}] = "A";
        KeyCodeNames[{char( 66)}] = "B";
        KeyCodeNames[{char( 67)}] = "C";
        KeyCodeNames[{char( 68)}] = "D";
        KeyCodeNames[{char( 69)}] = "E";
        KeyCodeNames[{char( 70)}] = "F";
        KeyCodeNames[{char( 71)}] = "G";
        KeyCodeNames[{char( 72)}] = "H";
        KeyCodeNames[{char( 73)}] = "I";
        KeyCodeNames[{char( 74)}] = "J";
        KeyCodeNames[{char( 75)}] = "K";
        KeyCodeNames[{char( 76)}] = "L";
        KeyCodeNames[{char( 77)}] = "M";
        KeyCodeNames[{char( 78)}] = "N";
        KeyCodeNames[{char( 79)}] = "O";
        KeyCodeNames[{char( 80)}] = "P";
        KeyCodeNames[{char( 81)}] = "Q";
        KeyCodeNames[{char( 82)}] = "R";
        KeyCodeNames[{char( 83)}] = "S";
        KeyCodeNames[{char( 84)}] = "T";
        KeyCodeNames[{char( 85)}] = "U";
        KeyCodeNames[{char( 86)}] = "V";
        KeyCodeNames[{char( 87)}] = "W";
        KeyCodeNames[{char( 88)}] = "X";
        KeyCodeNames[{char( 89)}] = "Y";
        KeyCodeNames[{char( 90)}] = "Z";
        KeyCodeNames[{char( 91)}] = "[";
        KeyCodeNames[{char( 92)}] = "\\";
        KeyCodeNames[{char( 93)}] = "]";
        KeyCodeNames[{char( 94)}] = "^";
        KeyCodeNames[{char( 95)}] = "_";
        KeyCodeNames[{char( 96)}] = "`";
        KeyCodeNames[{char( 97)}] = "a";
        KeyCodeNames[{char( 98)}] = "b";
        KeyCodeNames[{char( 99)}] = "c";
        KeyCodeNames[{char(100)}] = "d";
        KeyCodeNames[{char(101)}] = "e";
        KeyCodeNames[{char(102)}] = "f";
        KeyCodeNames[{char(103)}] = "g";
        KeyCodeNames[{char(104)}] = "h";
        KeyCodeNames[{char(105)}] = "i";
        KeyCodeNames[{char(106)}] = "j";
        KeyCodeNames[{char(107)}] = "k";
        KeyCodeNames[{char(108)}] = "l";
        KeyCodeNames[{char(109)}] = "m";
        KeyCodeNames[{char(110)}] = "n";
        KeyCodeNames[{char(111)}] = "o";
        KeyCodeNames[{char(112)}] = "p";
        KeyCodeNames[{char(113)}] = "q";
        KeyCodeNames[{char(114)}] = "r";
        KeyCodeNames[{char(115)}] = "s";
        KeyCodeNames[{char(116)}] = "t";
        KeyCodeNames[{char(117)}] = "u";
        KeyCodeNames[{char(118)}] = "v";
        KeyCodeNames[{char(119)}] = "w";
        KeyCodeNames[{char(120)}] = "x";
        KeyCodeNames[{char(121)}] = "y";
        KeyCodeNames[{char(122)}] = "z";
        KeyCodeNames[{char(123)}] = "{";
        KeyCodeNames[{char(124)}] = "|";
        KeyCodeNames[{char(125)}] = "}";
        KeyCodeNames[{char(126)}] = "~";

        return is_initialized_val = true;
    }

    // ----------------------------------------------------------------------------------- console functions

    [[nodiscard]] TKeyCode GetKey() {
        TKeyCode res;

        #if defined(_WIN32)

            do {
                int i = _getch();
                int c = static_cast<char>(i);
                if (c != i)
                {
                    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
                    break;
                }
                res << c;
            }
            while (_kbhit());

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

    TKeyCode WaitKeyCodes(
        ::std::set<TKeyCode> const& codes,
        TString              const& msg /* = {} */)
    {
        outstream << msg;
        outstream.flush();
        while (true) {
            auto kc = codes.find(GetKey());
            if (kc != codes.end()) return *kc;
        }
    }

    TKeyCode WaitAnyKey(
        TString const& msg /* = {} */)
    {
        outstream << msg;
        outstream.flush();
        return GetKey();
    }

    TKeyCode WaitReactToKeyCodes(
        ::std::set<TKeyCode> const& codes,
        TKeyCodeReactions    const& reactions /* = DefaultKeyCodeReactions */,
        TString              const& msg       /* = {} */)
    {
        auto kc = WaitKeyCodes(codes, msg);
        auto react = reactions.find(kc);
        if (react != reactions.end()) react->second();
        return kc;
    }

    TKeyCode WaitReactToAnyKey(
        TKeyCodeReactions const& reactions /* = DefaultKeyCodeReactions */,
        TString           const& msg       /* = {} */)
    {
        auto kc = WaitAnyKey(msg);
        auto react = reactions.find(kc);
        if (react != reactions.end()) react->second();
        return kc;
    }

} // namespace console
} // namespace sib
