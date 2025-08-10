#include "sib_console.h"

namespace sib {

    // ---------------------------------------------------------------------- TKeyCode

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

    std::string TKeyCode::name() const
    {
        if (size() == 0) return "";

        auto it = KeyCodeNames.find(*this);
        if (it != KeyCodeNames.end()) return it->second;

        std::string res;
        if (size() == 1) {
            char ch = (*this)[0];
            if ((static_cast<unsigned char>(ch) < 128) and isprint(static_cast<unsigned char>(ch))) res = ch;
            else res = "\\" + std::to_string(static_cast<unsigned char>(ch));
        }
        else {
            res = "\\" + std::to_string(static_cast<unsigned char>((*this)[0]));
            for (size_t i = 1; i < size(); ++i) {
                res += "\\" + std::to_string(static_cast<unsigned char>((*this)[i]));
            }
        }
        return res;
    }

    #ifdef _WIN32
        TKeyCode KC_ENTER(13);
        TKeyCode KC_ESC  (27);

        TKeyCode KC_F1 (0  ,  59);
        TKeyCode KC_F2 (0  ,  60);
        TKeyCode KC_F3 (0  ,  61);
        TKeyCode KC_F4 (0  ,  62);
        TKeyCode KC_F5 (0  ,  63);
        TKeyCode KC_F6 (0  ,  64);
        TKeyCode KC_F7 (0  ,  65);
        TKeyCode KC_F8 (0  ,  66);
        TKeyCode KC_F9 (0  ,  67);
        TKeyCode KC_F10(0  ,  68);
        TKeyCode KC_F11(224, 133);
        TKeyCode KC_F12(224, 134);

        TKeyCode KC_NUM_INSERT   (0, 82);
        TKeyCode KC_NUM_DELETE   (0, 83);
        TKeyCode KC_NUM_HOME     (0, 71);
        TKeyCode KC_NUM_END      (0, 79);
        TKeyCode KC_NUM_PAGE_UP  (0, 73);
        TKeyCode KC_NUM_PAGE_DOWN(0, 81);

        TKeyCode KC_INSERT   (224, 82);
        TKeyCode KC_DELETE   (224, 83);
        TKeyCode KC_HOME     (224, 71);
        TKeyCode KC_END      (224, 79);
        TKeyCode KC_PAGE_UP  (224, 73);
        TKeyCode KC_PAGE_DOWN(224, 81);

        TKeyCode KC_NUM_LEFT (0, 75);
        TKeyCode KC_NUM_RIGHT(0, 77);
        TKeyCode KC_NUM_UP   (0, 72);
        TKeyCode KC_NUM_DOWN (0, 80);

        TKeyCode KC_LEFT (224, 75);
        TKeyCode KC_RIGHT(224, 77);
        TKeyCode KC_UP   (224, 72);
        TKeyCode KC_DOWN (224, 80);
    #else
        TKeyCode KC_ENTER('\n'  );
        TKeyCode KC_ESC  ('\033');

        TKeyCode KC_F1 ('\033', '[', '1', '1');
        TKeyCode KC_F2 ('\033', '[', '1', '2');
        TKeyCode KC_F3 ('\033', '[', '1', '3');
        TKeyCode KC_F4 ('\033', '[', '1', '4');
        TKeyCode KC_F5 ('\033', '[', '1', '5');
        TKeyCode KC_F6 ('\033', '[', '1', '7');
        TKeyCode KC_F7 ('\033', '[', '1', '8');
        TKeyCode KC_F8 ('\033', '[', '1', '9');
        TKeyCode KC_F9 ('\033', '[', '2', '0');
        TKeyCode KC_F10('\033', '[', '2', '1');
        TKeyCode KC_F11('\033', '[', '2', '3');
        TKeyCode KC_F12('\033', '[', '2', '4');

        TKeyCode KC_INSERT   ('\033', '[', '2', '~');
        TKeyCode KC_DELETE   ('\033', '[', '3', '~');
        TKeyCode KC_HOME     ('\033', '[', 'H'     );
        TKeyCode KC_END      ('\033', '[', 'F'     );
        TKeyCode KC_PAGE_UP  ('\033', '[', '5', '~');
        TKeyCode KC_PAGE_DOWN('\033', '[', '6', '~');

        TKeyCode KC_NUM_INSERT   ('\033', '[', '2', '~');
        TKeyCode KC_NUM_DELETE   ('\033', '[', '3', '~');
        TKeyCode KC_NUM_HOME     ('\033', '[', 'H'     );
        TKeyCode KC_NUM_END      ('\033', '[', 'F'     );
        TKeyCode KC_NUM_PAGE_UP  ('\033', '[', '5', '~');
        TKeyCode KC_NUM_PAGE_DOWN('\033', '[', '6', '~');

        TKeyCode KC_LEFT ('\033', '[', 'D');
        TKeyCode KC_RIGHT('\033', '[', 'C');
        TKeyCode KC_UP   ('\033', '[', 'A');
        TKeyCode KC_DOWN ('\033', '[', 'B');

        TKeyCode KC_NUM_LEFT ('\033', '[', 'D');
        TKeyCode KC_NUM_RIGHT('\033', '[', 'C');
        TKeyCode KC_NUM_UP   ('\033', '[', 'A');
        TKeyCode KC_NUM_DOWN ('\033', '[', 'B');

    #endif

    TKeyCodeNames     KeyCodeNames            {};
    TKeyCodeReactions DefaultKeyCodeReactions {};
    bool ConsoleLibDefaultInitialization = []()
        {
            KeyCodeNames[KC_ENTER        ] = "ENTER"        ;
            KeyCodeNames[KC_ESC          ] = "ESC"          ;
        
            KeyCodeNames[KC_F1           ] = "F1"           ; 
            KeyCodeNames[KC_F2           ] = "F2"           ; 
            KeyCodeNames[KC_F3           ] = "F3"           ; 
            KeyCodeNames[KC_F4           ] = "F4"           ; 
            KeyCodeNames[KC_F5           ] = "F5"           ; 
            KeyCodeNames[KC_F6           ] = "F6"           ; 
            KeyCodeNames[KC_F7           ] = "F7"           ; 
            KeyCodeNames[KC_F8           ] = "F8"           ; 
            KeyCodeNames[KC_F9           ] = "F9"           ; 
            KeyCodeNames[KC_F10          ] = "F10"          ; 
            KeyCodeNames[KC_F11          ] = "F11"          ; 
            KeyCodeNames[KC_F12          ] = "F12"          ; 
        
            KeyCodeNames[KC_NUM_INSERT   ] = "NUM_INSERT"   ; 
            KeyCodeNames[KC_NUM_DELETE   ] = "NUM_DELETE"   ; 
            KeyCodeNames[KC_NUM_HOME     ] = "NUM_HOME"     ; 
            KeyCodeNames[KC_NUM_END      ] = "NUM_END"      ; 
            KeyCodeNames[KC_NUM_PAGE_UP  ] = "NUM_PAGE_UP"  ; 
            KeyCodeNames[KC_NUM_PAGE_DOWN] = "NUM_PAGE_DOWN"; 
        
            KeyCodeNames[KC_INSERT       ] = "INSERT"       ; 
            KeyCodeNames[KC_DELETE       ] = "DELETE"       ; 
            KeyCodeNames[KC_HOME         ] = "HOME"         ; 
            KeyCodeNames[KC_END          ] = "END"          ; 
            KeyCodeNames[KC_PAGE_UP      ] = "PAGE_UP"      ; 
            KeyCodeNames[KC_PAGE_DOWN    ] = "PAGE_DOWN"    ; 
        
            KeyCodeNames[KC_NUM_LEFT     ] = "NUM_LEFT"     ; 
            KeyCodeNames[KC_NUM_RIGHT    ] = "NUM_RIGHT"    ; 
            KeyCodeNames[KC_NUM_UP       ] = "NUM_UP"       ; 
            KeyCodeNames[KC_NUM_DOWN     ] = "NUM_DOWN"     ; 
        
            KeyCodeNames[KC_LEFT         ] = "LEFT"         ; 
            KeyCodeNames[KC_RIGHT        ] = "RIGHT"        ; 
            KeyCodeNames[KC_UP           ] = "UP"           ; 
            KeyCodeNames[KC_DOWN         ] = "DOWN"         ; 

            return true;
        }();

} // namespace sib