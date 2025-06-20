﻿#include <iostream>

#include <string>
#include <vector>

#include "sib_support.h"
#include "sib_wrapper.h"
#include "sib_unit_test.h"

using namespace std::string_literals;

using sib::TNullPtr;
using sib::TValue;
using sib::TPointer;
using sib::TArray;

// -----------------------------------------------------------------------------------


#define MY_ASSERTIONS_FOR_TYPES(type, p, lp, mbi, f, lf, a, c)                              \
                                                                                            \
    static_assert( p   std::is_pointer_v       <type>, #type" "#p  " is_pointer"       );   \
    static_assert( lp  sib::is_like_pointer_v  <type>, #type" "#lp " is_like_pointer"  );   \
    static_assert( mbi sib::may_be_indirect_v  <type>, #type" "#mbi" may_be_indirect"  );   \
    static_assert( f   std::is_function_v      <type>, #type" "#f  " is_function"      );   \
    static_assert( lf  sib::is_like_function_v <type>, #type" "#lf " is_like_function" );   \
    static_assert( a   std::is_array_v         <type>, #type" "#c  " is_array_v"       );   \
    static_assert( c   sib::is_container_v     <type>, #type" "#c  " is_container"     );   \

#define _MAFT MY_ASSERTIONS_FOR_TYPES

// -----------------------------------------------------------------------------------

static int foo(float f) { return static_cast<int>(f); }
static int bar(float f) { return static_cast<int>(-f); }

using TFn = decltype(foo);

//                          |---------------|---------------|---------------|---------------|---------------|---------------|---------------|
//                          |    pointer    | like_pointer  |may_be_indirect|   function    | like_function |     array     |   container   |
//                          |---------------|---------------|---------------|---------------|---------------|---------------|---------------|
_MAFT(std::nullptr_t        ,      not      ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(TNullPtr              ,      not      ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(void*                 ,               ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(TValue<void*>         ,      not      ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(TPointer<void>        ,      not      ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(int*                  ,               ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(TValue<int*>          ,      not      ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(TPointer<int>         ,      not      ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(TFn                   ,      not      ,      not      ,      not      ,               ,               ,      not      ,      not      )
_MAFT(TFn*                  ,               ,               ,               ,      not      ,               ,      not      ,      not      )
_MAFT(TValue<TFn*>          ,      not      ,               ,               ,      not      ,               ,      not      ,      not      )
_MAFT(TPointer<TFn>         ,      not      ,               ,               ,      not      ,               ,      not      ,      not      )
_MAFT(TFn**                 ,               ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(TValue<TFn**>         ,      not      ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(TPointer<TFn*>        ,      not      ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(int[5]                ,      not      ,               ,               ,      not      ,      not      ,               ,               )
_MAFT(std::array<int _ 5>   ,      not      ,      not      ,      not      ,      not      ,      not      ,      not      ,               )
_MAFT(TArray<int _ 5>       ,      not      ,               ,               ,      not      ,      not      ,      not      ,               )
//                          |---------------|---------------|---------------|---------------|---------------|---------------|---------------|

enum TEnum { e_1 = 1, e_2, e_3, e_4, e_5 };

enum class TEnumClass { e_1 = 1, e_2, e_3, e_4, e_5 };

enum class TEnumClass123 : unsigned char { _1 = 1, _2, _3 };

using BVector = std::vector<int>;
class HVector : public BVector {
public:
    HVector() : BVector() {}
    HVector(BVector& b) : BVector(b) {}
    HVector(BVector&& b) : BVector(std::move(b)) {}

    template <typename... Args>
    HVector(Args&& ...args) : BVector(std::forward<Args>(args)...) {};

    template <typename T>
    HVector(std::initializer_list<T> inilist) : BVector(inilist) {}
};

using BArray = std::array<int, 5>;
class HArray : public BArray {};

template <typename T>
struct _function
{
    template <typename... Args>
    _function(Args&&... args) {}
};

struct MyClass
{
    MyClass() = default;
    MyClass(MyClass const&) = default;
    MyClass(MyClass&&) = default;
    ~MyClass() { std::cout << "~MyClass()"; }
    operator char const* () const { return "MyClass"; }
};

MyClass(MyClass::* MyClassConstrPtr)();
int (MyClass::* MyClassMethodPtr)(int);

using TMyClassConstrPtr = decltype(MyClassConstrPtr);
using TMyClassConstr = std::remove_pointer_t<TMyClassConstrPtr>;

using TMyClassMethodPtr = decltype(MyClassMethodPtr);
using TMyClassMethod = std::remove_pointer_t<TMyClassMethodPtr>;

struct MyStruct : sib::TWrapper<int>, sib::TWrapper<TEnum>, sib::TWrapper<TEnumClass123> {};

// MAIN ------------------------------------------------------------------------------

#define TEST_NULLPTR
#define TEST_VALUE
#define TEST_POINTER
#define TEST_ARRAY
#define TEST_WRAPPER
#define TEST_UNIQUE_TUPLE

#define CHAKE_BOOL(expr) std::cout << #expr << " = " << (expr) << '\n';

int main()
{
    setlocale(LC_ALL, "Russian");

    int const i1 = 1111;
    decltype(auto) i2 = i1;
    decltype(auto) w1 = sib::to_wrap(i1);
    PRN(w1);

    TValue<TFn*> v1 = foo;
    v1 = bar;
    PRN(v1);

    TFn* const fp = foo;
    decltype(auto) v2 = sib::to_wrap(fp);
    //v2 = bar;
    PRN(v2);

    //sib::WaitAnyKey();
    //return 0;

#ifdef TEST_NULLPTR
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                             TNullPtr                                             \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        DEFA(TNullPtr, ptr, = nullptr, TNullPtr);
        EXE(int* ip = ptr);
        PRN(ptr);
        PRN(ip);
        END;

        PRN(ptr == ip);
        END;
    } {
        BEG;
        DEFA(TNullPtr const, ptr, , TNullPtr const);
        EXE(int const* ip = ptr);
        PRN(ptr);
        PRN(ip);
        PRN(ptr == ip);
        END;
    } {
        BEG;
        DEF(std::nullptr_t, null_p, = nullptr);
        DEFA(TNullPtr, ptr, = null_p, TNullPtr);
        PRN(null_p);
        PRN(ptr);
        END;

        PRN(ptr == null_p);
        END;
    } {
        BEG;
        DEF(std::nullptr_t const, null_p, {});
        DEFA(TNullPtr, ptr, = null_p, TNullPtr);
        PRN(null_p);
        PRN(ptr);
        END;
    } {
        BEG;
        DEFA(TNullPtr const, ptr, (nullptr), TNullPtr const);
        EXE(int* ip = ptr);
        PRN(ptr);
        PRN(ip);
        END;
    } {
        BEG;
        DEFA(TNullPtr, ptr, (nullptr), TNullPtr);
        EXE(int* ip = ptr);
        PRN(ptr);
        PRN(ip);
        END;
    }
#endif TEST_NULLPTR

#ifdef TEST_VALUE
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                              TValue                                              \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
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

        DEF(TValue<int>, vi0,         );
        DEF(TValue<int>, vi1, { 111 } );
        DEF(TValue<int>, vi2, ( 222 ) );
        DEF(TValue<int>, vi3, = 333   );
        DEF(TValue<int>, vi4, = 444.f );
        DEF(TValue<int>, vi5, { _i5 } );
        DEF(TValue<int>, vi6, ( _i6 ) );
        DEF(TValue<int>, vi7, = _i7   );
        DEF(TValue<int>, vi8, ( _f8 ) );
        DEF(TValue<int>, vi9, = _f9   );

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
        DEFA(TValue, v0,         { i0 }, TValue<int>);
        DEFA(TValue, v1,         ( i1 ), TValue<int>);
        DEFA(TValue, v2, =         i2  , TValue<int>);
        DEFA(auto  , v3, = TValue{ i3 }, TValue<int>);
        DEFA(auto  , v4, = TValue( i4 ), TValue<int>);
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
        DEF(TValue<int*>, p1, = &arr[0]);
        DEF(TValue<int*>, p2, = &arr[1]);
        EXE(auto aaa = p2 - p1);
        PRN(arr);
        PRN(p1);
        PRN(p2);
        PRN(aaa);
        END;
    } {
        BEG;
        EXE(TValue<int> v = 11);
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
        DEFA(TValue, v0,         { ic0 }, TValue<int>);
        DEFA(TValue, v1,         ( ic1 ), TValue<int>);
        DEFA(TValue, v2, =         ic2  , TValue<int>);
        DEFA(auto  , v3, = TValue{ ic3 }, TValue<int>);
        DEFA(auto  , v4, = TValue( ic4 ), TValue<int>);
        PRN(v0);
        PRN(v1);
        PRN(v2);
        PRN(v3);
        PRN(v4);
        END;
    } {
        BEG;
        DEFA(TValue      , v0 , = 42   , TValue<int>);
        DEFA(TValue      , v1 , = v0   , TValue<int>);
        DEF(TValue<float>, v2 , { v0 } );
        DEF(TValue<float>, v3 , ( v0 ) );
        DEF(TValue<float>, v4 , = v0   );
        DEF(TValue<int>  , v5 , { v2 } );
        DEF(TValue<int>  , v6 , ( v2 ) );
        DEF(TValue<int>  , v7 , = v2   );
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
        DEFA(auto, v1, = TValue{ 10  }, TValue<int   >);
        DEFA(auto, v2, = TValue{ 2.2 }, TValue<double>);
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
        DEF(TValue, v, = 4.4);
        DEF(std::vector<int>, vec, (v));
        PRN(v);
        PRN(vec);
        END;
    } {
        BEG;
        DEF(TValue, v1, = 11);
        DEF(TValue, v2, = 1.1 + v1 + 2);
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
        DEF(int, i, = 111);
        DEF(int*, ip, = &i);
        DEF(TValue<int*>, v1, = &i);
        DEFA(TValue, v2, = &i, TValue<int*>);
        PRN(i);
        PRN(ip);
        PRN(v1);
        PRN(v2);
        END;
    } {
        BEG;
        DEF(TEnum, E, = e_1);
        DEF(TValue<TEnum> const, V, = E);
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
        PRNAS(e2, sib::underlying_type_t<decltype(e2)>);
        PRNAS(e3, sib::underlying_type_t<decltype(e3)>);
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
        DEF(TValue<TEnum        >, e1, =               e_1);
        DEF(TValue<TEnumClass   >, e2, = TEnumClass::e_2);
        DEF(TValue<TEnumClass123>, e3, = TEnumClass123::_3);
        PRN(e1);
        PRNAS(e2, sib::underlying_type_t<decltype(e2)>);
        PRNAS(e3, sib::underlying_type_t<decltype(e3)>);
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
        PRNAS(e2, sib::underlying_type_t<decltype(e2)>);
        PRNAS(e3, sib::underlying_type_t<decltype(e3)>);
        END;

        DEF(TValue, ve1, = e1);
        DEF(TValue, ve2, = e2);
        DEF(TValue, ve3, = e3);
        PRN(ve1);
        PRNAS(ve2, sib::underlying_type_t<decltype(ve2)>);
        PRNAS(ve3, sib::underlying_type_t<decltype(ve3)>);
        END;

        EXE(ve1 = e_3);
        EXE(ve2 = TEnumClass::e_5);
        EXE(ve3 = TEnumClass123::_1);
        PRN(ve1);
        PRNAS(ve2, sib::underlying_type_t<decltype(ve2)>);
        PRNAS(ve3, sib::underlying_type_t<decltype(ve3)>);
        END;
    } {
        BEG;
        DEF(TEnum, e1, { e_1 });
        DEF(TEnumClass, e2, { 100 });
        DEF(TEnumClass123, e3, { 100 });
        PRN(e1);
        PRNAS(e2, sib::underlying_type_t<decltype(e2)>);
        PRNAS(e3, sib::underlying_type_t<decltype(e3)>);
        END;

        using qqq = typename TValue<TEnum>::underlying_t;

        DEF(TValue<TEnum        >, ve1, { e_1 });
        DEF(TValue<TEnumClass   >, ve2, { 100 });
        DEF(TValue<TEnumClass123>, ve3, { 100 });
        PRN(e1);
        PRNAS(e2, sib::underlying_type_t<decltype(e2)>);
        PRNAS(e3, sib::underlying_type_t<decltype(e3)>);
        END;
    } {
        BEG;
        DEFA(auto, i, = 1, int);
        DEFA(TValue, val, = 2, TValue<int>);
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
        DEFA(TValue, val, = 111, TValue<int>);
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
#endif TEST_VALUE

#ifdef TEST_POINTER
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                             TPointer                                             \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        DEF(TFn*, ptrFoo, (foo));
        DEFA(TPointer, PointerFoo, (foo), TPointer<TFn>);
        PRN(foo);
        PRN(ptrFoo);
        PRN(PointerFoo);
        END;
        EXE(auto r1 = (*foo)(4.2f));
        EXE(auto r2 = (*ptrFoo)(4.2f));
        EXE(auto r3 = (*PointerFoo)(4.2f));
        PRN(r1);
        PRN(r2);
        PRN(r3);
        END;
        EXE(r1 = foo(1.2f));
        EXE(r2 = ptrFoo(1.2f));
        EXE(r3 = PointerFoo(1.2f));
        PRN(r1);
        PRN(r2);
        PRN(r3);
        END;
    } {
        BEG;
        DEF(int, i, = 10);
        DEFA(TPointer, ptr, (&i), TPointer<int>);
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
        DEFA(TPointer, ptr, (&i), TPointer<int const>);
        PRN(ptr);
        PRN(&i);
        END;
    } {
        BEG;
        DEF(TPointer<int>, ptr, );
        PRN(ptr);
        END;
        DEF(int, i, = 10);
        EXE(ptr = &i);
        EXE(*ptr = 222222);
        PRN(i);
        END;
    } {
        BEG;
        DEF(TPointer<int const>, ptr, );
        PRN(ptr);
        END;
        DEF(int, i, = 10);
        EXE(ptr = &i);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(TPointer<int>, ptr, );
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
        DEFA(TPointer, ptr, (&s), TPointer<std::string>);
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
        DEFA(TPointer, ptr, (&s1), TPointer<std::string const>);
        PRN(ptr);
        END;
        EXE(ptr = &s2);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(std::string, s, = "qwerty");
        DEF(TPointer<std::string const> const, ptr, (&s));
        PRN(ptr);
        END;
        EXE(s = "!!!!!!!!");
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(int, i, = 111);
        DEFA(TPointer const, cptr_i, = &i, TPointer<int> const);
        DEF(TPointer<int const>, ptr_ci, );
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
        DEFA(TPointer, ptr, ("qwerty"), TPointer<char const>);
        PRN(ptr);
        END;
        EXE(ptr = "!!!!!!!!!!!!!!");
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(TPointer<std::nullptr_t>, ptr, );
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(TPointer<void>, ptr, (nullptr));
        PRN(ptr);
        END;
        DEF(int, i, = 777);
        EXE(ptr = &i);
        PRN(ptr);
        END;
        EXE(ptr = nullptr);
        DEF(int*, ip, = static_cast<int*>(ptr));
        PRN(ip);
        END;
    } {
        BEG;
        DEF(std::vector<int>, vec, );
        DEF(TPointer, ptr, = &vec);
        EXE(ptr->push_back(11));
        PRN(vec);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(std::vector<int> const, vec, { 1 _ 2 _ 3 });
        DEF(TPointer, ptr, = &vec);
        PRN(vec);
        PRN(ptr);
        PRN(ptr->size());
        END;
    } {
        BEG;
        DEF(TPointer, ptr, = new MyClass);
        PRN(ptr);
        EXE(delete ptr);
        END;
    } {
        BEG;
        DEF(int, i, = 10);
        DEF(TPointer, ptr, = &i);
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
        EXE(ptr = ptr - 3);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(int, i1, = 10);
        DEF(int, i2, = 20);
        DEF(TPointer, ptr1, = &i1);
        DEF(TPointer, ptr2, = &i2);
        PRN(i1);
        PRN(i2);
        PRN(&i1);
        PRN(&i2);
        PRN(ptr1);
        PRN(ptr2);
        PRN(i2 - i1);
        PRN(*ptr2 - *ptr1);
        PRN(&i2 - &i1);
        PRN(ptr2 - ptr1);
        END;
    }
#endif TEST_POINTER

#ifdef TEST_ARRAY
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                              TArray                                              \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        DEF(int, i, = 222);
        DEF(int const, ic, = 111);
        DEFA(TArray, arr, (i _ ic _ 333 _ ic _ i), TArray<int _ 5>);
        PRN(arr);
        END;
    } {
        BEG;
        DEFA(TArray, arr, (1 _ 2 _ 3), TArray<int _ 3>);
        DEFA(TArray, arrarr, (arr _ arr _ arr), TArray<TArray<int _ 3> _ 3>);
        PRN(arr);
        PRN(arrarr);
        END;
    } {
        BEG;
        DEFA(TArray, arr, { 1 _ 2 _ 3 _ 4 _ 5 }, TArray<int _ 5>);
        PRN(arr);
        END;
        EXE(arr = { 8 _ 7 _ 6 _ 5 _ 4 });
        PRN(arr);
        END;
    } {
        BEG;
        DEF(static constexpr char, cech, = '1');
        DEF(static constexpr char, cech0, = '\0');
        DEFA(TArray, arr, { cech _ cech _ cech _ cech0 }, TArray<char _ 4>);
        PRN(arr);
        END;
    } {
        BEG;
        DEFA(TArray, arr, { '1' _ '2' _ '3' _ '4' _ '5' _ '\0' }, TArray<char _ 6>);
        PRN(arr);
        END;
        EXE(arr = "87654");
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char, ch10, [10] = "12345");
        DEFA(TArray, arr, = ch10, TArray<char _ 10>);
        PRN(ch10);
        PRN(arr);
        END;
        EXE(arr = "123456789");
        EXE(strcpy_s(ch10, arr.data()));
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
        EXE(std::cout << ch10 << '\n');
        EXE(std::cout << arr << '\n');
        END;
    } {
        BEG;
        DEFA(TArray, arr1, = "12345", TArray<char _ 6>);
        DEFA(TArray, arr2, = "54321", TArray<char _ 6>);
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
        DEFA(TArray, arr, = cha, TArray<char _ 4>);
        PRN(cha);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char const, cch, = '1');
        DEF(char const, cch0, = '\0');
        DEFA(TArray, arr, { cch _ cch _ cch _ cch0 }, TArray<char _ 4>);
        PRN(arr);
        END;
        EXE(using ch_c_arr4 = char const [4]);
        EXE(ch_c_arr4& ch_c_arr4_r = arr);
        PRN(ch_c_arr4_r);
        END;
    } {
        BEG;
        DEF(TArray<char const _ 4>, arr, = "999");
        PRN(arr);
        END;
    } {
        BEG;
        DEFA(TArray, arr, = "999", TArray<char _ 4>);
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
        DEF(TArray<char const _ 7>, arr1, = "qwerty");
        PRN(arr1);
        END;
        DEF(TArray<char _ 7>, arr2, = arr1);
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
        DEFA(TArray, arr, = charr, TArray<char _ 7>);
        PRN(charr);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char, charr, [10] = "qwerty111");
        DEFA(TArray, arr, = charr, TArray<char _ 10>);
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
        DEF(TArray<int _ 3>, arr,);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(std::array, starr, = { 1 _ 2 _ 3 _ 4 });
        DEFA(TArray, arr, = starr, TArray<int _ 4>);
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
        DEFA(TArray, arr, = sarr, TArray<char _ 4>);
        PRN(sarr);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(TArray, arr, (std::array{ 'f' _ 'f' _ 'f' _ 'f' }));
        PRN(arr);
        END;
    }
#endif TEST_ARRAY

#ifdef TEST_WRAPPER
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                              TWrapper                                              \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        DEF(int, i, = 1);
        DEF(auto, w, = sib::to_wrap(i));
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
        DEF(auto, w, = sib::to_wrap(ic));
        PRN(w);
        END;
    } {
        BEG;
        DEF(int const, ic, = 1);
        DEF(decltype(auto), w, = sib::to_wrap(ic));
        PRN(w);
        END;
    } {
        BEG;
        DEF(auto, w, = sib::to_wrap(1));
        PRN(w);
        END;
    } {
        BEG;
        DEF(auto, w, = sib::to_wrap(nullptr));
        PRN(w);
        END;
    } {
        BEG;
        DEF(auto, w1, = sib::to_wrap(char(100)));
        DEF(auto, w2, = sib::to_wrap(w1));
        char c = w2;
        DEF(auto, s, = "111"s + c);
        PRN(w1);
        PRN(w2);
        PRN(s);
        END;
    } {
        BEG;
        EXE(int i = 2 + sib::to_wrap(3));
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
        EXE(std::wcout << w << '\n');
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
        BEG;
        DEF(MyStruct, tmp, {});
        PRN(tmp);
        PRN(int(tmp));
        PRN(TEnum(tmp));
        PRNAS(TEnumClass123(tmp), sib::underlying_type_t<TEnumClass123>);
        END;
        EXE(static_cast<int&>(tmp) = 444);
        EXE(static_cast<TEnum&>(tmp) = e_3);
        EXE(static_cast<TEnumClass123&>(tmp) = TEnumClass123::_1);
        PRN(tmp);
        PRN(int(tmp));
        PRN(TEnum(tmp));
        PRNAS(TEnumClass123(tmp), sib::underlying_type_t<TEnumClass123>);
        END;
        DEF(TEnum, e, = tmp);
        PRN(e);
        END;
        EXE(e = e_2);
        EXE(TValue<TEnum> ve);
        EXE(ve = e);
        PRN(e);
        PRN(ve);
        END;
    } {
        BEG;
        DEF(auto, w, = sib::to_wrap(L"123456789ффф"s));
        PRN(w);
        END;
    } {
        BEG;
        DEFA(char, c, [] = "123", char[4]);
        END;
    } {
        BEG;
        END;
    }
#endif TEST_WRAPPER

#ifdef TEST_UNIQUE_TUPLE
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                           TUniqueTuple                                           \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        struct A {};
        struct B {};
        struct C {};
        struct D {};
        struct E {};

        static_assert(std::is_same_v<
            sib::MakeUniqueTuple<std::vector<int>, bool, std::string>,
            sib::MakeUniqueTuple<std::string, std::vector<int, std::allocator<int>>, bool>
        >);

        static_assert(std::is_same_v<
            sib::MakeUniqueTuple<A, B, C>,
            sib::MakeUniqueTuple<B, C, A>
        >);

        sib::MakeUniqueTuple<A, B, C> tmp1;
        sib::MakeUniqueTuple<C, B, A> tmp2;
        sib::MakeUniqueTuple<B, A, C> tmp3;

        sib::MakeUniqueTuple<std::vector<int>, bool, std::string> tmp4;
        sib::MakeUniqueTuple<std::string, std::vector<int, std::allocator<int>>, bool> tmp5;

        static_assert(std::is_same_v<decltype(tmp1), decltype(tmp2)>);
        static_assert(std::is_same_v<decltype(tmp2), decltype(tmp3)>);
        static_assert(std::is_same_v<decltype(tmp3), decltype(tmp1)>);

        static_assert(std::is_same_v<decltype(tmp4), decltype(tmp5)>);
        static_assert(std::is_same_v<decltype(tmp5), decltype(tmp4)>);

        std::cout << sib::type_name(tmp1) << '\n';
        std::cout << sib::type_name(tmp2) << '\n';
        std::cout << sib::type_name(tmp3) << '\n';
        std::cout << sib::type_name(tmp4) << '\n';
        std::cout << sib::type_name(tmp5) << '\n';

        END;
    } {
        BEG;
        END;
    } {
        BEG;
        END;
    }
#endif TEST_UNIQUE_TUPLE

    std::cout << "\n";
    sib::WaitKeyCodes(
        { sib::KC_ENTER, sib::KC_ESC },
        "     --- END ---\n"
        " press [Enter, Esc]..."
    );
    return 0;
}
