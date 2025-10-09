#include "sib_unit_test.h"
#include <clocale>
#include <string>
#include "sib_support.h"

// #include <iostream>
// #include <string>

#define TEST_CONSOLE
//#define TEST_TYPE_TRAITS
//#define TEST_TYPES_PACK
//#define TEST_TYPES_LIST
//#define TEST_NULLPTR
//#define TEST_VALUE
//#define TEST_POINTER
//#define TEST_ARRAY
//#define TEST_WRAPPER
//#define TEST_UNIQUE_TUPLE

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
using namespace std::string_literals;

// MAIN ------------------------------------------------------------------------------

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF8");
    
    #ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif

    SIB_SCOPE_GUARD(
        sib::console::WaitAnyKey(
            "     --- END ---\n"
            "   press any key..."
        );
        sib::console::outstream << std::endl;
    );

    sib::debug::CONTAINER_DISCLOSURE_LENGTH = 32;
    sib::debug::Init();
    
    #ifdef TEST_CONSOLE
        sib::debug::Tests.emplace("01 console", test_console);
    #endif
    
    #ifdef TEST_TYPE_TRAITS
        sib::debug::Tests.emplace("02 type_traits", test_type_traits);
    #endif
    
    #ifdef TEST_TYPES_PACK
        sib::debug::Tests.emplace("03 type_traits::types_pack", test_types_pack);
    #endif
    
    #ifdef TEST_TYPES_LIST
        sib::debug::Tests.emplace("04 type_traits::types_list", test_types_list);
    #endif
    
    #ifdef TEST_NULLPTR
        sib::debug::Tests.emplace("05 wrapper::TNullPtr", test_TNullPtr);
    #endif
    
    #ifdef TEST_VALUE
        sib::debug::Tests.emplace("06 wrapper::TValue", test_TValue);
    #endif
    
    #ifdef TEST_POINTER
        sib::debug::Tests.emplace("07 wrapper::TPointer", test_TPointer);
    #endif
    
    #ifdef TEST_ARRAY
        sib::debug::Tests.emplace("08 wrapper::TArray", test_TArray);
    #endif
    
    #ifdef TEST_WRAPPER
        sib::debug::Tests.emplace("09 wrapper::TWrapper", test_TWrapper);
    #endif
    
    #ifdef TEST_UNIQUE_TUPLE
        sib::debug::Tests.emplace("10 unique_tuple", test_TUniqueTuple);
    #endif
    
    sib::debug::RunAllTest();
    sib::debug::outstream << sib::debug::ReportText() << "\n";
    
    return 0;
}
