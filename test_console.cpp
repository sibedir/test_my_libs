#include "test_console.h"
#include "sib_unit_test.h"

DEF_TEST(test_console)
{

    sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                            sib_console                                             ");
    MSG("****************************************************************************************************");
    MSG("");

    {
        BEG;
        #ifdef _WIN32
            sib::console::KeyCodeNames[{(char)128}] = "А";
            sib::console::KeyCodeNames[{(char)129}] = "Б";
            sib::console::KeyCodeNames[{(char)130}] = "В";
            sib::console::KeyCodeNames[{(char)131}] = "Г";
            // ...
        #else
            sib::console::KeyCodeNames[{'\xD0', '\x90'}] = "А";
            sib::console::KeyCodeNames[{'\xD0', '\x91'}] = "Б";
            sib::console::KeyCodeNames[{'\xD0', '\x92'}] = "В";
            sib::console::KeyCodeNames[{'\xD0', '\x93'}] = "Г";
            // ...
        #endif

        sib::debug::outstream << "Wait [Esc]...\n";
        sib::console::TKeyCode kc;
        do {
            kc = sib::console::WaitAnyKey();
            sib::debug::outstream << "[" << sib::debug::TString(kc.name()) << "]\n";
        } while (kc != sib::console::KC_ESC);
        END;
    }

    sib::debug::outstream << std::endl;
    return 0;
}
