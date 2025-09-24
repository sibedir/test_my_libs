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
    
            KC_ENTER << 13;
            KC_ESC   << 27;
    
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
    
            KC_NUM_INSERT    << char(0) << char (82);
            KC_NUM_DELETE    << char(0) << char (83);
            KC_NUM_HOME      << char(0) << char (71);
            KC_NUM_END       << char(0) << char (79);
            KC_NUM_PAGE_UP   << char(0) << char (73);
            KC_NUM_PAGE_DOWN << char(0) << char (81);
    
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


        // ----------------------------------------------------------------------------------- default KeyCodeNames registration

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

        return is_initialized_val = true;
    }

} // namespace console
} // namespace sib
