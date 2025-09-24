#define _CRT_SECURE_NO_WARNINGS

#include "test_wrapper.h"

#include "sib_unit_test.h"
#include "sib_wrapper.h"

#include <cstring>
#include <string>
#include <cstdlib>

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_TNullPtr)
{
    ::sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                              TNullPtr                                              ");
    MSG("****************************************************************************************************");
    MSG("");

    {
        BEG;
        DEFA(sib::TNullPtr, ptr, = nullptr, ::sib::TNullPtr);
        EXE(int* ip = ptr);
        PRN(ptr);
        PRN(ip);
        END;
        PRN(ptr == ip);
        END;
    } {
        BEG;
        DEFA(sib::TNullPtr const, ptr, , ::sib::TNullPtr const);
        EXE(int const* ip = ptr);
        PRN(ptr);
        PRN(ip);
        PRN(ptr == ip);
        END;
    } {
        BEG;
        DEF(std::nullptr_t, null_p, = nullptr);
        DEFA(sib::TNullPtr, ptr, = null_p, ::sib::TNullPtr);
        PRN(null_p);
        PRN(ptr);
        END;

        PRN(ptr == null_p);
        END;
    } {
        BEG;
        DEF(std::nullptr_t const, null_p, {});
        DEFA(sib::TNullPtr, ptr, = null_p, ::sib::TNullPtr);
        PRN(null_p);
        PRN(ptr);
        END;
    } {
        BEG;
        DEFA(sib::TNullPtr const, ptr, (nullptr), ::sib::TNullPtr const);
        EXE(int* ip = ptr);
        PRN(ptr);
        PRN(ip);
        END;
    } {
        BEG;
        DEFA(sib::TNullPtr, ptr, (nullptr), ::sib::TNullPtr);
        EXE(int* ip = ptr);
        PRN(ptr);
        PRN(ip);
        END;
    }

    ::sib::debug::outstream << ::std::endl;
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

#define _ ,

// ---------------------------------------------------------------------------------------------------------------------

enum TEnum { e_1 = 1, e_2, e_3, e_4, e_5 };

enum class TEnumClass { e_1 = 1, e_2, e_3, e_4, e_5 };

enum class TEnumClass123 : unsigned char { _1 = 1, _2, _3 };

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_TValue)
{
    ::sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                               TValue                                               ");
    MSG("****************************************************************************************************");
    MSG("");
    
    {
        BEG;
        EXE(int   _i5 = 555);
        EXE(int   _i6 = 666);
        EXE(int   _i7 = 777);
        EXE(float _f8 = 888);
        EXE(float _f9 = 999);
        DEF(int, i0, {}      );
        DEF(int, i1, { 111 } );
        DEF(int, i2, ( 222 ) );
        DEF(int, i3, = 333   );
        DEF(int, i4, = 444.f );
        DEF(int, i5, { _i5 } );
        DEF(int, i6, ( _i6 ) );
        DEF(int, i7, = _i7   );
        DEF(int, i8, ( _f8 ) );
        DEF(int, i9, = _f9   );
        PRN(i0);
        PRN(i1);
        PRN(i2);
        PRN(i3);
        PRN(i4);
        PRN(i5);
        PRN(i6);
        PRN(i7);
        PRN(i8);
        PRN(i9);

        DEF(sib::TValue<int>, vi0,         );
        DEF(sib::TValue<int>, vi1, { 111 } );
        DEF(sib::TValue<int>, vi2, ( 222 ) );
        DEF(sib::TValue<int>, vi3, = 333   );
        DEF(sib::TValue<int>, vi4, = 444.f );
        DEF(sib::TValue<int>, vi5, { _i5 } );
        DEF(sib::TValue<int>, vi6, ( _i6 ) );
        DEF(sib::TValue<int>, vi7, = _i7   );
        DEF(sib::TValue<int>, vi8, ( _f8 ) );
        DEF(sib::TValue<int>, vi9, = _f9   );

        PRN(vi0);
        PRN(vi1);
        PRN(vi2);
        PRN(vi3);
        PRN(vi4);
        PRN(vi5);
        PRN(vi6);
        PRN(vi7);
        PRN(vi8);
        PRN(vi9);
        END;
    } {
        BEG;
        EXE(int i0 = 0);
        EXE(int i1 = 1);
        EXE(int i2 = 2);
        EXE(int i3 = 3);
        EXE(int i4 = 4);
        DEFA(sib::TValue, v0,         { i0 }, ::sib::TValue<int>);
        DEFA(sib::TValue, v1,         ( i1 ), ::sib::TValue<int>);
        DEFA(sib::TValue, v2, =         i2  , ::sib::TValue<int>);
        DEFA(auto  , v3, = ::sib::TValue{ i3 }, ::sib::TValue<int>);
        DEFA(auto  , v4, = ::sib::TValue( i4 ), ::sib::TValue<int>);
        PRN(v0);
        PRN(v1);
        PRN(v2);
        PRN(v3);
        PRN(v4);
        END;

        EXE(auto aaa = v1 + v2);
        PRN(aaa);
        END;

        EXE(i0 = v0);
        EXE(i1 = v1);
        EXE(i2 = v2);
        EXE(i3 = v3);
        EXE(i4 = v4);
        PRN(i0);
        PRN(i1);
        PRN(i2);
        PRN(i3);
        PRN(i4);
        END;
    } {
        BEG;
        DEF(int, arr, [3] = { 1 _ 2 _ 3 });
        DEF(sib::TPointer<int>, p1, = &arr[0]);
        DEF(sib::TPointer<int>, p2, = &arr[1]);
        //EXE(auto aaa = p2 - p1);
        PRN(arr);
        PRN(p1);
        PRN(p2);
        //PRN(aaa);
        END;
    } {
        BEG;
        EXE(sib::TValue<int> v = 11);
        EXE(std::vector<int> vec_i(v));
        PRN(v);
        PRN(vec_i);
        END;
    } {
        BEG;
        EXE(int const ic0 = 0);
        EXE(int const ic1 = 1);
        EXE(int const ic2 = 2);
        EXE(int const ic3 = 3);
        EXE(int const ic4 = 4);
        DEFA(sib::TValue, v0,         { ic0 }, ::sib::TValue<int>);
        DEFA(sib::TValue, v1,         ( ic1 ), ::sib::TValue<int>);
        DEFA(sib::TValue, v2, =         ic2  , ::sib::TValue<int>);
        DEFA(auto  , v3, = ::sib::TValue{ ic3 }, ::sib::TValue<int>);
        DEFA(auto  , v4, = ::sib::TValue( ic4 ), ::sib::TValue<int>);
        PRN(v0);
        PRN(v1);
        PRN(v2);
        PRN(v3);
        PRN(v4);
        END;
    } {
        BEG;
        DEFA(sib::TValue      , v0 , = 42   , ::sib::TValue<int>);
        DEFA(sib::TValue      , v1 , = v0   , ::sib::TValue<int>);
        DEF(sib::TValue<float>, v2 , { v0 } );
        DEF(sib::TValue<float>, v3 , ( v0 ) );
        DEF(sib::TValue<float>, v4 , = v0   );
        DEF(sib::TValue<int>  , v5 , { v2 } );
        DEF(sib::TValue<int>  , v6 , ( v2 ) );
        DEF(sib::TValue<int>  , v7 , = v2   );
        DEF(int const    , i  , = v2   );
        DEF(float const  , f  , = v0   );
        PRN(v0);
        PRN(v1);
        PRN(v2);
        PRN(v3);
        PRN(v4);
        PRN(v5);
        PRN(v6);
        PRN(v7);
        PRN(i);
        PRN(f);
        END;
    } {
        BEG;
        DEFA(auto, v1, = ::sib::TValue{ 10  }, ::sib::TValue<int   >);
        DEFA(auto, v2, = ::sib::TValue{ 2.2 }, ::sib::TValue<double>);
        PRN(v1);
        PRN(v2);
        END;

        EXE(v1 = v2);
        PRN(v1);
        EXE(v2 = v1 * 2);
        PRN(v2);
        END;
    } {
        BEG;
        DEF(auto, v1, = 4.4);
        DEF(sib::TValue, v2, = 4.4);
        DEF(std::vector<int>, vec1, (v1));
        DEF(std::vector<int>, vec2, (v2));
        PRN(v1);
        PRN(v2);
        PRN(vec1);
        PRN(vec2);
        END;
    } {
        BEG;
        DEF(sib::TValue, v1, = 11);
        DEF(sib::TValue, v2, = 1.1 + v1 + 2);
        PRN(v1);
        PRN(v2);
        END;

        DEF(int&, ir, = v1);
        EXE(ir = 111);
        PRN(ir);
        PRN(v1);
        END;
    } {
        BEG;
        DEF(TEnum, E, = e_1);
        DEF(sib::TValue<TEnum> const, V, = E);
        PRN(E);
        PRN(V);
        END;

        EXE(E = e_2);
        PRN(E);
        END;

        EXE(E = V);
        PRN(E);
        END;
    } {
        BEG;
        DEF(TEnum, e1, =               e_1);
        DEF(TEnumClass, e2, = TEnumClass::e_2);
        DEF(TEnumClass123, e3, = TEnumClass123::_3);
        PRN(e1);
        PRNAS(e2, ::std::underlying_type_t<decltype(e2)>);
        PRNAS(e3, ::std::underlying_type_t<decltype(e3)>);
        END;

        EXE(std::vector<int> vec_i1(e1));
        EXE(std::vector<int> vec_i2(static_cast<size_t>(e2)));
        EXE(std::vector<int> vec_i3(static_cast<size_t>(e3)));
        PRN(vec_i1);
        PRN(vec_i2);
        PRN(vec_i3);
        END;
    } {
        BEG;
        DEF(sib::TValue<TEnum        >, e1, =               e_1);
        DEF(sib::TValue<TEnumClass   >, e2, = TEnumClass::e_2);
        DEF(sib::TValue<TEnumClass123>, e3, = TEnumClass123::_3);
        PRN(e1);
        PRNAS(e2, decltype(e2)::underlying_type);
        PRNAS(e3, decltype(e3)::underlying_type);
        END;

        EXE(std::vector<int> vec_i1(e1));
        EXE(std::vector<int> vec_i2(static_cast<size_t>(e2)));
        EXE(std::vector<int> vec_i3(static_cast<size_t>(e3)));
        PRN(vec_i1);
        PRN(vec_i2);
        PRN(vec_i3);
        END;
    } {
        BEG;
        DEF(TEnum, e1, =               e_1);
        DEF(TEnumClass, e2, = TEnumClass::e_2);
        DEF(TEnumClass123, e3, = TEnumClass123::_3);
        PRN(e1);
        PRNAS(e2, ::std::underlying_type_t<decltype(e2)>);
        PRNAS(e3, ::std::underlying_type_t<decltype(e3)>);
        END;

        DEF(sib::TValue, ve1, = e1);
        DEF(sib::TValue, ve2, = e2);
        DEF(sib::TValue, ve3, = e3);
        PRN(ve1);
        PRNAS(ve2, decltype(ve2)::underlying_type);
        PRNAS(ve3, decltype(ve3)::underlying_type);
        END;

        EXE(ve1 = e_3);
        EXE(ve2 = TEnumClass::e_5);
        EXE(ve3 = TEnumClass123::_1);
        PRN(ve1);
        PRNAS(ve2, decltype(ve2)::underlying_type);
        PRNAS(ve3, decltype(ve3)::underlying_type);
        END;
    } {
        BEG;
        DEF(TEnum, e1, { e_1 });
        DEF(TEnumClass, e2, { 100 });
        DEF(TEnumClass123, e3, { 100 });
        PRN(e1);
        PRNAS(e2, ::std::underlying_type_t<decltype(e2)>);
        PRNAS(e3, ::std::underlying_type_t<decltype(e3)>);
        END;

        DEF(sib::TValue<TEnum        >, ve1, { e_1 });
        DEF(sib::TValue<TEnumClass   >, ve2, { 100 });
        DEF(sib::TValue<TEnumClass123>, ve3, { 100 });
        PRN(e1);
        PRNAS(e2, ::std::underlying_type_t<decltype(e2)>);
        PRNAS(e3, ::std::underlying_type_t<decltype(e3)>);
        END;
    } {
        BEG;
        DEFA(auto, i, = 1, int);
        DEFA(sib::TValue, val, = 2, ::sib::TValue<int>);
        DEF(float&&, fcr1, = i);
        DEF(float&&, fcr2, = val);
        PRN(i);
        PRN(val);
        PRN(fcr1);
        PRN(fcr2);
        END;

        EXE(val = 42);
        PRN(val);
        PRN(fcr2);
        EXE(fcr2 = 777777);
        PRN(val);
        PRN(fcr2);
        END;
    } {
        BEG;
        DEFA(sib::TValue, val, = 111, ::sib::TValue<int>);
        DEF(int&, ir, = val);
        PRN(val);
        PRN(ir);
        END;

        EXE(val = 0);
        PRN(val);
        PRN(ir);
        END;

        EXE(ir = 42);
        PRN(val);
        PRN(ir);
        END;
    }

    ::sib::debug::outstream << ::std::endl;
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

int foo(float f) { return static_cast<int>(f); }

using TFn = decltype(foo);

// ---------------------------------------------------------------------------------------------------------------------

struct MyClass
{
    MyClass()                { ::sib::debug::outstream << "MyClass()\n"               ; }
    MyClass(MyClass const &) { ::sib::debug::outstream << "MyClass(MyClass const &)\n"; }
    MyClass(MyClass      &&) { ::sib::debug::outstream << "MyClass(MyClass &&)\n"     ; }

    template <typename T>
    MyClass(T) { ::sib::debug::outstream << "MyClass(" << ::sib::type_name<T>() << ")\n"; }

    virtual ~MyClass() { ::sib::debug::outstream << "~MyClass()\n"; }
    virtual void aaa() {}
    operator char const* () const { return "MyClass"; }
};

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_TPointer)
{
    ::sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                              TPointer                                              ");
    MSG("****************************************************************************************************");
    MSG("");

    {
        BEG;
        DEF(TFn*, pFoo, (foo));
        DEFA(sib::TPointer, PFoo, (foo), ::sib::TPointer<TFn>);
        PRN(foo);
        PRN(pFoo);
        PRN(PFoo);
        END;

        EXE(auto r1 = (*foo)(4.2f));
        EXE(auto r2 = (*pFoo)(4.2f));
        EXE(auto r3 = (*PFoo)(4.2f));
        PRN(r1);
        PRN(r2);
        PRN(r3);
        END;

        EXE(r1 = foo(1.2f));
        EXE(r2 = pFoo(1.2f));
        EXE(r3 = PFoo(1.2f));
        PRN(r1);
        PRN(r2);
        PRN(r3);
        END;
    } {
        BEG;
        DEF(int, i, = 10);
        DEFA(sib::TPointer, ptr, (&i), ::sib::TPointer<int>);
        PRN(ptr);
        PRN(&i);
        END;

        EXE(*ptr = 999);
        EXE(int i2 = *ptr);
        PRN(ptr);
        PRN(&i);
        PRN(&i2);
        END;
    } {
        BEG;
        DEF(int const, i, = 10);
        DEFA(sib::TPointer, ptr, (&i), ::sib::TPointer<int const>);
        PRN(ptr);
        PRN(&i);
        END;
    } {
        BEG;
        DEF(int, i, = 10);
        DEF(int*, ip, = &i);
        DEF(sib::TPointer<int>, iP, = &i);
        PRN(ip);
        PRN(iP);
        END;

        PRN(i);
        EXE(ip = &i);
        EXE(iP = &i);
        EXE(*ip = 222222);
        PRN(i);
        EXE(*iP = 0);
        PRN(i);
        END;
    } {
        BEG;
        DEF(int, i, = 10);
        DEF(sib::TPointer<int const>, ptr, { &i });
        PRN(ptr);
        END;

        EXE(ptr = &i);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(sib::TPointer<int>, ptr, = 0);
        PRN(ptr);
        END;

        EXE(ptr = new int);
        PRN(ptr);
        END;

        EXE(*ptr = 99);
        PRN(ptr);
        END;

        DEF(int*, ip, = ptr);
        EXE(ptr = static_cast<int*>(operator new(sizeof(int))));
        PRN(ptr);
        PRN(ip);
        EXE(delete ip);
        END;
        
        DEF(void*, vp, = static_cast<void*>(ptr));
        PRN(vp);
        EXE(delete ptr);
        END;

        DEF(int, i, = 78778787);
        EXE(ptr = &i);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(std::string, s, = "qwerty");
        DEFA(sib::TPointer, ptr, (&s), ::sib::TPointer<::std::string>);
        PRN(ptr);
        END;

        EXE(s = "!!!!!!!!!!!!");
        PRN(ptr);
        END;

        EXE((*ptr)[2] = '_');
        PRN(s);
        END;
    } {
        BEG;
        DEF(std::string const, s1, = "qwerty");
        DEF(std::string const, s2, = "QAZXSWEDC");
        DEFA(sib::TPointer, ptr, (&s1), ::sib::TPointer<::std::string const>);
        PRN(ptr);
        END;

        EXE(ptr = &s2);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(std::string, s, = "qwerty");
        DEF(sib::TPointer<::std::string const> const, ptr, (&s));
        PRN(ptr);
        END;

        EXE(s = "!!!!!!!!");
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(int, i, = 111);
        DEFA(sib::TPointer const, cptr_i, = &i, ::sib::TPointer<int> const);
        DEF(sib::TPointer<int const>, ptr_ci, = &i);
        PRN(cptr_i);
        PRN(ptr_ci);
        END;

        EXE(ptr_ci = cptr_i);
        EXE(i = 222);
        PRN(cptr_i);
        PRN(ptr_ci);
        END;
    } {
        BEG;
        DEFA(sib::TPointer, ptr, ("qwerty"), ::sib::TPointer<char const>);
        PRN(ptr);
        END;

        EXE(ptr = "!!!!!!!!!!!!!!");
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(sib::TPointer<::std::nullptr_t>, ptr, {});
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(void*, vp, = nullptr);
        DEF(sib::TPointer<void>, vP, = nullptr);
        PRN(vp);
        PRN(vP);
        END;

        DEF(int, i, = 777);
        EXE(vp = &i);
        EXE(vP = &i);
        PRN(vp);
        PRN(vP);
        END;

        EXE(vp = nullptr);
        EXE(vP = nullptr);
        DEF(int*, ip1, = static_cast<int*>(vp));
        DEF(int*, ip2, = static_cast<int*>(vP));
        PRN(ip1);
        PRN(ip2);
        END;
    } {
        BEG;
        DEF(std::vector<int>, vec, );
        DEF(sib::TPointer, ptr, = &vec);
        EXE(ptr->push_back(11));
        PRN(vec);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(std::vector<int> const, vec, { 1 _ 2 _ 3 });
        DEF(sib::TPointer, ptr, = &vec);
        PRN(vec);
        PRN(ptr);
        PRN(ptr->size());
        END;
    } {
        BEG;
        DEF(sib::TPointer, ptr, = new MyClass);
        PRN(ptr);
        EXE(delete ptr);
        END;

        DEF(MyClass, o,);
        EXE(ptr = &o);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(int, i, = 10);
        DEF(sib::TPointer, ptr, = &i);
        PRN(ptr);
        END;

        PRN(ptr++);
        END;

        PRN(--ptr);
        END;

        EXE(--ptr);
        PRN(ptr);
        END;

        EXE(ptr += 4);
        PRN(ptr);
        END;

        //EXE(ptr = ptr - 3);
        //PRN(ptr);
        END;
    } {
        BEG;
        DEF(int, i1, = 10);
        DEF(int, i2, = 20);
        DEF(sib::TPointer, ptr1, = &i1);
        DEF(sib::TPointer, ptr2, = &i2);
        PRN(i1);
        PRN(i2);
        PRN(&i1);
        PRN(&i2);
        PRN(ptr1);
        PRN(ptr2);
        PRN(i2 - i1);
        PRN(*ptr2 - *ptr1);
        PRN(&i2 - &i1);
        //PRN(ptr2 - ptr1);
        END;
    }

    ::sib::debug::outstream << ::std::endl;
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_TArray)
{
    ::sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                               TArray                                               ");
    MSG("****************************************************************************************************");
    MSG("");
    {
        BEG;
        DEF(int, i, = 222);
        DEF(int const, ic, = 111);
        DEFA(sib::TArray, arr, (i _ ic _ 333 _ ic _ i), ::sib::TArray<int _ 5>);
        PRN(arr);
        END;
    } {
        BEG;
        DEFA(sib::TArray, arr, (1 _ 2 _ 3), ::sib::TArray<int _ 3>);
        DEFA(sib::TArray, arrarr, (arr _ arr _ arr), ::sib::TArray<::sib::TArray<int _ 3> _ 3>);
        PRN(arr);
        PRN(arrarr);
        END;
    } {
        BEG;
        DEFA(sib::TArray, arr, ( 1, 2, 3, 4, 5 ), ::sib::TArray<int _ 5>);
        PRN(arr);
        END;

        EXE(arr = { 8 _ 7 _ 6 _ 5 _ 4 });
        PRN(arr);
        END;
    } {
        BEG;
        DEF(static constexpr char, cech, = '1');
        DEF(static constexpr char, cech0, = '\0');
        DEFA(sib::TArray, arr, ( cech _ cech _ cech _ cech0 ), ::sib::TArray<char _ 4>);
        PRN(arr);
        END;
    } {
        BEG;
        DEFA(sib::TArray, arr, ( '1' _ '2' _ '3' _ '4' _ '5' _ '\0' ), ::sib::TArray<char _ 6>);
        PRN(arr);
        END;

        EXE(arr = "87654");
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char, ch10, [10] = "12345");
        DEFA(sib::TArray, arr, = ch10, ::sib::TArray<char _ 10>);
        PRN(ch10);
        PRN(arr);
        END;

        EXE(arr = "123456789");
        EXE(strncpy(ch10, arr.data(), 10));
        PRN(arr);
        PRN(ch10);
        END;

        EXE(ch10[3] = 0);
        PRN(ch10);
        PRN(arr);
        EXE(arr = ch10);
        PRN(ch10);
        PRN(arr);
        END;

        EXE(sib::debug::outstream << ch10 << '\n');
        EXE(sib::debug::outstream << arr << '\n');
        END;
    } {
        BEG;
        DEFA(sib::TArray, arr1, = "12345", ::sib::TArray<char _ 6>);
        DEFA(sib::TArray, arr2, = "54321", ::sib::TArray<char _ 6>);
        PRN(arr1);
        PRN(arr2);
        END;

        EXE(arr1 = arr2);
        EXE(arr2[1] = '0');
        PRN(arr1);
        PRN(arr2);
        END;
    } {
        BEG;
        DEF(char const, cha, [] = "111");
        DEFA(sib::TArray, arr, = cha, ::sib::TArray<char _ 4>);
        PRN(cha);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char const, cch, = '1');
        DEF(char const, cch0, = '\0');
        DEFA(sib::TArray, arr, ( cch _ cch _ cch _ cch0 ), ::sib::TArray<char _ 4>);
        PRN(arr);
        END;

        EXE(using ch_c_arr4 = char const [4]);
        EXE(ch_c_arr4& ch_c_arr4_r = arr);
        PRN(ch_c_arr4_r);
        END;
    } {
        BEG;
        DEF(sib::TArray<char const _ 4>, arr, = "999");
        PRN(arr);
        END;
    } {
        BEG;
        DEFA(sib::TArray, arr, = "999", ::sib::TArray<char _ 4>);
        PRN(arr);
        END;

        EXE(using ch_c_arr4 = char const [4]);
        EXE(ch_c_arr4 & ch_c_arr4_r = arr);
        EXE(arr[0] = '_');
        PRN(ch_c_arr4_r);
        END;

        EXE(using ch_arr4 = char[4]);
        EXE(ch_arr4 & ch_arr4_r = arr);
        EXE(ch_arr4_r[2] = '_');
        PRN(arr);
        PRN(ch_c_arr4_r);
        END;
    } {
        BEG;
        DEF(sib::TArray<char const _ 7>, arr1, = "qwerty");
        PRN(arr1);
        END;

        DEF(sib::TArray<char _ 7>, arr2, = arr1);
        PRN(arr2);
        END;

        DEF(char const *, ch, = arr1);
        EXE(arr2[1] = '_');
        PRN(arr1);
        PRN(arr2);
        PRN(ch);
        END;

        EXE(using ch_arr = char [7]);
        EXE(ch_arr& ch_arr_r = arr2);
        EXE(arr2 = "asdfgh");
        PRN(arr2);
        PRN(ch_arr_r);
        END;

        EXE(ch_arr_r[4] = '_');
        PRN(arr2);
        PRN(ch_arr_r);
        END;

        EXE(*arr2.begin() = '_');
        PRN(arr2);
        PRN(ch_arr_r);
        END;
    } {
        BEG;
        DEF(char, charr, [7] = "qwerty");
        DEFA(sib::TArray, arr, = charr, ::sib::TArray<char _ 7>);
        PRN(charr);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char, charr, [10] = "qwerty111");
        DEFA(sib::TArray, arr, = charr, ::sib::TArray<char _ 10>);
        PRN(arr);
        END;

        EXE(arr = "!!!!!!000");
        PRN(arr);
        END;

        EXE(arr[2] = '_');
        PRN(arr);
        END;
    } {
        BEG;
        DEF(sib::TArray<int _ 3>, arr,);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(std::array, starr, = { 1 _ 2 _ 3 _ 4 });
        DEFA(sib::TArray, arr, = starr, ::sib::TArray<int _ 4>);
        PRN(arr);
        END;

        EXE(arr = { 8 _ 8 _ 8 _ 8 });
        PRN(arr);
        END;

        EXE(arr[2] = 0);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(std::array, sarr, { 'f' _ 'f' _ 'f' _ 'f' });
        DEFA(sib::TArray, arr, = sarr, ::sib::TArray<char _ 4>);
        PRN(sarr);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(sib::TArray, arr, (std::array{ 'f' _ 'f' _ 'f' _ 'f' }));
        PRN(arr);
        END;
    }

    ::sib::debug::outstream << ::std::endl;
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

using namespace ::std::string_literals;

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_TWrapper)
{
   
    ::sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                              TWrapper                                              ");
    MSG("****************************************************************************************************");
    MSG("");

    {
        BEG;
        DEF(int, i, = 1);
        DEF(auto, w, = ::sib::to_wrap(i));
        PRN(i);
        PRN(w);
        END;

        EXE(w = 2);
        EXE(i = w);
        PRN(i);
        PRN(w);
        END;

        EXE(i = 3);
        EXE(w = i);
        PRN(i);
        PRN(w);
        END;
    } {
        BEG;
        DEF(int const, ic, = 1);
        DEF(auto, w, = ::sib::to_wrap(ic));
        PRN(w);
        END;
    } {
        BEG;
        DEF(int const, ic, = 1);
        DEF(decltype(auto), w, = ::sib::to_wrap(ic));
        PRN(w);
        END;
    } {
        BEG;
        DEF(auto, w, = ::sib::to_wrap(1));
        PRN(w);
        END;
    } {
        BEG;
        DEF(auto, w, = ::sib::to_wrap(nullptr));
        PRN(w);
        END;
    } {
        BEG;
        DEF(auto, w1, = ::sib::to_wrap(char(100)));
        DEF(auto, w2, = ::sib::to_wrap(w1));
        char c = w2;
        DEF(auto, s, = "111"s + c);
        PRN(w1);
        PRN(w2);
        PRN(s);
        END;
    } {
        BEG;
        EXE(int i = 2 + ::sib::to_wrap(3));
        PRN(i);
        END;
    } {
        BEG;
        DEF(int, i, = 2);
        DEF(auto, w, (sib::to_wrap(&i)));
        PRN(i);
        PRN(w);
        END;

        EXE(*w = 3);
        PRN(i);
        PRN(w);
        END;
    } {
        BEG;
        DEF(auto, c, = '+');
        DEF(auto, p, = &c);
        DEF(auto, w, (sib::to_wrap(p)));
        DEF(auto, s, = "_"s + *w);
        PRN(c);
        PRN(p);
        PRN(w);
        PRN(s);
        END;
    } {
        BEG;
        DEF(char const, a, [] = "1234\0005678");
        DEF(decltype(auto), w, (sib::to_wrap(a)));
        PRN(a);
        PRN(w);
        EXE(sib::debug::outstream << w << '\n');
        END;
    } {
        BEG;
        DEF(std::vector const, v, { 1 _ 2 _ 3 _ 4 _ 5 });
        DEF(decltype(auto), w, (sib::to_wrap(v)));
        PRN(v);
        PRN(w);
        END;
    } {
        BEG;
        DEF(decltype(auto), w, (sib::to_wrap(foo)));
        PRN(w);
        PRN(w(11.1));
        END;
    } {
        struct MyStruct : ::sib::TWrapper<int>, ::sib::TWrapper<TEnum>, ::sib::TWrapper<TEnumClass123> {};

        BEG;
        DEF(MyStruct, tmp, {});
        PRN(tmp);
        PRN(int(tmp));
        PRN(TEnum(tmp));
        PRNAS(TEnumClass123(tmp), ::std::underlying_type_t<TEnumClass123>);
        END;

        EXE(static_cast<int&>(tmp) = 444);
        EXE(static_cast<TEnum&>(tmp) = e_3);
        EXE(static_cast<TEnumClass123&>(tmp) = TEnumClass123::_1);
        PRN(tmp);
        PRN(int(tmp));
        PRN(TEnum(tmp));
        PRNAS(TEnumClass123(tmp), ::std::underlying_type_t<TEnumClass123>);
        END;

        DEF(TEnum, e, = tmp);
        PRN(e);
        END;

        EXE(e = e_2);
        EXE(sib::TValue<TEnum> ve);
        EXE(ve = e);
        PRN(e);
        PRN(ve);
        END;
    } {
        BEG;
        DEF(auto, w, = ::sib::to_wrap(L"123456789ффф"s));
        PRN(w);
        END;
    } {
        BEG;
        DEFA(char, c, [] = "123", char[4]);
        END;
    } {
        BEG;
        DEF(std::nullptr_t, nptr, {});
        DEF(decltype(auto), w, = ::sib::to_wrap(nptr));
        PRN(nptr);
        PRN(w);
        END;
    } {
        BEG;
        DEF(std::nullptr_t const, nptr, {});
        DEF(decltype(auto), w, = ::sib::to_wrap(nptr));
        PRN(nptr);
        PRN(w);
        END;
    }

    ::sib::debug::outstream << ::std::endl;
    return 0;
}
