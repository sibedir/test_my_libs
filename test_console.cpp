#define DISABLE_WARNING_NON_STANDARD_FUNC_PTR_CONVERSION

#include "test_console.h"
#include "sib_unit_test.h"

DEF_TEST(test_console)
{
    sib::debug::Init();
    
    {
        sib ::debug ::TBufer __buf__;
        __buf__ << "m   ";
        __buf__ << sib ::debug ::string();
        __buf__ << "\n";
        sib ::debug ::under_lock_print(__buf__.str());
    }
    auto sss = sib::debug::string("qwerty");

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                            sib_console                                             ");
    MSG("****************************************************************************************************");
    MSG("");

    {
        BEG;
        #ifdef _WIN32
            sib::KeyCodeNames[{(char)128}] = "А";
            sib::KeyCodeNames[{(char)129}] = "Б";
            sib::KeyCodeNames[{(char)130}] = "В";
            sib::KeyCodeNames[{(char)131}] = "Г";
            // ...
        #else
            sib::KeyCodeNames[{'\xD0', '\x90'}] = "А";
            sib::KeyCodeNames[{'\xD0', '\x91'}] = "Б";
            sib::KeyCodeNames[{'\xD0', '\x92'}] = "В";
            sib::KeyCodeNames[{'\xD0', '\x93'}] = "Г";
            // ...
        #endif
    
        sib::debug::outstream << "Wait [Esc]...\n";
        sib::TKeyCode kc;
        do {
            kc = sib::WaitAnyKey();
            sib::debug::outstream << "[" << sib::debug::string(kc.name()) << "]\n";
        } while (kc != sib::KC_ESC);
        END;
    }

    sib::debug::outstream << std::endl;
    return 0;
}
