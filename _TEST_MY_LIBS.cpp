#include "sib_unit_test.h"
#include <clocale>
#include <string>

// #include <iostream>
// #include <string>

#define TEST_CONSOLE
#define TEST_TYPE_TRAITS
#define TEST_TYPES_PACK
#define TEST_TYPES_LIST
#define TEST_NULLPTR
#define TEST_VALUE
#define TEST_POINTER
#define TEST_ARRAY
#define TEST_WRAPPER
#define TEST_UNIQUE_TUPLE

#if defined(TEST_CONSOLE)
    #include "test_console.h"
#endif

#if defined(TEST_TYPE_TRAITS) || defined(TEST_TYPES_PACK) || defined(TEST_TYPES_LIST)
    #include "test_type_traits.h"
#endif

#if defined(TEST_NULLPTR) || defined(TEST_VALUE) || defined(TEST_POINTER) || defined(TEST_ARRAY) || defined(TEST_WRAPPER)
    #include "test_wrapper.h"
#endif

#if defined(TEST_UNIQUE_TUPLE)
    #include "test_unique_typle.h"
#endif

#ifdef _WIN32
    #include <Windows.h>
#endif

#include "sib_wrapper.h"
using namespace ::std::string_literals;

// MAIN ------------------------------------------------------------------------------

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF8");
    
    #ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif

    ::sib::debug::DISCLOSURE_STRING_LENGTH = 32;
    
    ::sib::debug::Init();
    
    using namespace ::sib::debug;
   
    SIB_SCOPE_GUARD(
        ::sib::console::WaitAnyKey(
            "     --- END ---\n"
            "   press any key..."
        );
        outstream << ::std::endl;
    );
    


    #ifdef TEST_CONSOLE
        Tests.emplace("console", test_console);
    #endif
    
    #ifdef TEST_TYPE_TRAITS
        Tests.emplace("type_traits", test_type_traits);
    #endif
    
    #ifdef TEST_TYPES_PACK
        Tests.emplace("type_traits::types_pack", test_types_pack);
    #endif
    
    #ifdef TEST_TYPES_LIST
        Tests.emplace("type_traits::types_list", test_types_list);
    #endif
    
    #ifdef TEST_NULLPTR
        Tests.emplace("wrapper::TNullPtr", test_TNullPtr);
    #endif
    
    #ifdef TEST_VALUE
        Tests.emplace("wrapper::TValue", test_TValue);
    #endif
    
    #ifdef TEST_POINTER
        Tests.emplace("wrapper::TPointer", test_TPointer);
    #endif
    
    #ifdef TEST_ARRAY
        Tests.emplace("wrapper::TArray", test_TArray);
    #endif
    
    #ifdef TEST_WRAPPER
        Tests.emplace("wrapper::TWrapper", test_TWrapper);
    #endif
    
    #ifdef TEST_UNIQUE_TUPLE
        Tests.emplace("unique_tuple", test_TUniqueTuple);
    #endif
    
    RunAllTest();

    TBufer buf;
    string border = "********************************************************************************************************\n";
    buf << border <<"                                                REPORT                                                  \n";
    int i = 0;
    for (auto test_it: Tests)
    {
        buf << border;
        ++i;
        buf << "TEST " << i << ": " << test_it.first << "\n";
        auto& test = test_it.second;
        
        switch (test.state()) {
            case TTestState::NotInitialized : buf << "Not initialized" << "\n"; break;
            case TTestState::NotCompleted   : buf << "Not completed"   << "\n"; break;
            case TTestState::Completed      : buf << "Completed"       << "\n"; break;
            default                         : buf << "Unknown state"   << "\n";
        }
        
        int l = 0, m = 0, w = 0, e = 0;
        for (auto rec_it: test.log()) {
            buf << rec_it.united_message() << "\n";
            ++l; 
            switch (rec_it.typ) {
                case TTestLogType::message : ++m; break;
                case TTestLogType::warning : ++w; break;
                case TTestLogType::error   : ++e; break;
            }
        }
        buf << "\nlog count: " << l
            << "  messages: "  << m
            << "  warnings: "  << w
            << "  errors: "    << e
            << "\n";
        
        border = "--------------------------------------------------------------------------------------------------------\n";
    }
    
    buf << "********************************************************************************************************\n\n";
    
    outstream << "\n" << buf.str();
    
    return 0;
}
