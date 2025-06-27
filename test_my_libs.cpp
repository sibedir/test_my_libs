#include <iostream>

#include <string>
#include <vector>
#include <tuple>

#include "sib_wrapper.h"
#include "sib_unique_tuple.h"
#include "sib_support.h"
#include "sib_unit_test.h"

using namespace std::string_literals;

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

static auto baz(int         const &) { return "do baz(int)"s;          }
static auto baz(double      const &) { return "do baz(double)"s;       }
static auto baz(std::string const &) { return "do baz(std::string&)"s; }

using TFn = decltype(foo);

//                          |---------------|---------------|---------------|---------------|---------------|---------------|---------------|
//                          |    pointer    | like_pointer  |may_be_indirect|   function    | like_function |     array     |   container   |
//                          |---------------|---------------|---------------|---------------|---------------|---------------|---------------|
_MAFT(std::nullptr_t        ,      not      ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(sib::TNullPtr         ,      not      ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(void*                 ,               ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(sib::TValue<void*>    ,      not      ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(sib::TPointer<void>   ,      not      ,               ,      not      ,      not      ,      not      ,      not      ,      not      )
_MAFT(int*                  ,               ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(sib::TValue<int*>     ,      not      ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(sib::TPointer<int>    ,      not      ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(TFn                   ,      not      ,      not      ,      not      ,               ,               ,      not      ,      not      )
_MAFT(TFn*                  ,               ,               ,               ,      not      ,               ,      not      ,      not      )
_MAFT(sib::TValue<TFn*>     ,      not      ,               ,               ,      not      ,               ,      not      ,      not      )
_MAFT(sib::TPointer<TFn>    ,      not      ,               ,               ,      not      ,               ,      not      ,      not      )
_MAFT(TFn**                 ,               ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(sib::TValue<TFn**>    ,      not      ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(sib::TPointer<TFn*>   ,      not      ,               ,               ,      not      ,      not      ,      not      ,      not      )
_MAFT(int[5]                ,      not      ,               ,               ,      not      ,      not      ,               ,               )
_MAFT(std::array<int _ 5>   ,      not      ,      not      ,      not      ,      not      ,      not      ,      not      ,               )
_MAFT(sib::TArray<int _ 5>  ,      not      ,               ,               ,      not      ,      not      ,      not      ,               )
//                          |---------------|---------------|---------------|---------------|---------------|---------------|---------------|



template <size_t... idx_>
consteval auto gen_TP_impl(std::index_sequence<idx_...>)
{
    return sib::type_pack< sib::int_tag<idx_> ... > {};
}

template <size_t... idx_>
consteval auto gen_TL_impl(std::index_sequence<idx_...>)
{
    return sib::type_list< sib::int_tag<idx_> ... > {};
}

template <size_t N> using gen_TP = decltype(gen_TP_impl(std::make_index_sequence<N>{}));
template <size_t N> using gen_TL = decltype(gen_TL_impl(std::make_index_sequence<N>{}));

template <typename... Ts>
inline std::string Types_to_Str()
{
    if constexpr (sizeof...(Ts) == 0)
    {
        return "<>";
    }
    else if constexpr (sizeof...(Ts) == 1)
    {
        return "<" + std::to_string(sib::types_first_t<Ts...>::value) + ">";
    }
    else
    {
        std::string res = "<";
        ((res += std::to_string(Ts::value) + ", "), ...);
        res[res.size() - 2] = '>';
        return res;
    }
}

template <template <typename...> typename Templ, typename... Ts>
    requires(std::is_base_of_v<sib::container_of_types, Templ<>>)
inline std::string Types_to_Str(Templ<Ts...> const &)
{
    return Types_to_Str<Ts...>();
}



struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

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

//#define TEST_NULLPTR
//#define TEST_VALUE
//#define TEST_POINTER
//#define TEST_ARRAY
//#define TEST_WRAPPER
//#define TEST_TYPE_PACK
//#define TEST_TYPE_LIST
#define TEST_UNIQUE_TUPLE

#define CHAKE_BOOL(expr) std::cout << #expr << " = " << (expr) << '\n';

int main()
{
    setlocale(LC_ALL, "Russian");

    //sib::WaitAnyKey();
    //return 0;

#ifdef TEST_NULLPTR
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                             sib::TNullPtr                                             \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        DEFA(sib::TNullPtr, ptr, = nullptr, sib::TNullPtr);
        EXE(int* ip = ptr);
        PRN(ptr);
        PRN(ip);
        END;

        PRN(ptr == ip);
        END;
    } {
        BEG;
        DEFA(sib::TNullPtr const, ptr, , sib::TNullPtr const);
        EXE(int const* ip = ptr);
        PRN(ptr);
        PRN(ip);
        PRN(ptr == ip);
        END;
    } {
        BEG;
        DEF(std::nullptr_t, null_p, = nullptr);
        DEFA(sib::TNullPtr, ptr, = null_p, sib::TNullPtr);
        PRN(null_p);
        PRN(ptr);
        END;

        PRN(ptr == null_p);
        END;
    } {
        BEG;
        DEF(std::nullptr_t const, null_p, {});
        DEFA(sib::TNullPtr, ptr, = null_p, sib::TNullPtr);
        PRN(null_p);
        PRN(ptr);
        END;
    } {
        BEG;
        DEFA(sib::TNullPtr const, ptr, (nullptr), sib::TNullPtr const);
        EXE(int* ip = ptr);
        PRN(ptr);
        PRN(ip);
        END;
    } {
        BEG;
        DEFA(sib::TNullPtr, ptr, (nullptr), sib::TNullPtr);
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
        std::cout << "                                              sib::TValue                                              \n";
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
        DEFA(sib::TValue, v0,         { i0 }, sib::TValue<int>);
        DEFA(sib::TValue, v1,         ( i1 ), sib::TValue<int>);
        DEFA(sib::TValue, v2, =         i2  , sib::TValue<int>);
        DEFA(auto  , v3, = sib::TValue{ i3 }, sib::TValue<int>);
        DEFA(auto  , v4, = sib::TValue( i4 ), sib::TValue<int>);
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
        DEF(sib::TValue<int*>, p1, = &arr[0]);
        DEF(sib::TValue<int*>, p2, = &arr[1]);
        EXE(auto aaa = p2 - p1);
        PRN(arr);
        PRN(p1);
        PRN(p2);
        PRN(aaa);
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
        DEFA(sib::TValue, v0,         { ic0 }, sib::TValue<int>);
        DEFA(sib::TValue, v1,         ( ic1 ), sib::TValue<int>);
        DEFA(sib::TValue, v2, =         ic2  , sib::TValue<int>);
        DEFA(auto  , v3, = sib::TValue{ ic3 }, sib::TValue<int>);
        DEFA(auto  , v4, = sib::TValue( ic4 ), sib::TValue<int>);
        PRN(v0);
        PRN(v1);
        PRN(v2);
        PRN(v3);
        PRN(v4);
        END;
    } {
        BEG;
        DEFA(sib::TValue      , v0 , = 42   , sib::TValue<int>);
        DEFA(sib::TValue      , v1 , = v0   , sib::TValue<int>);
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
        DEFA(auto, v1, = sib::TValue{ 10  }, sib::TValue<int   >);
        DEFA(auto, v2, = sib::TValue{ 2.2 }, sib::TValue<double>);
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
        DEF(int, i, = 111);
        DEF(int*, ip, = &i);
        DEF(sib::TValue<int*>, v1, = &i);
        DEFA(sib::TValue, v2, = &i, sib::TValue<int*>);
        PRN(i);
        PRN(ip);
        PRN(v1);
        PRN(v2);
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
        PRNAS(e2, std::underlying_type_t<decltype(e2)>);
        PRNAS(e3, std::underlying_type_t<decltype(e3)>);
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
        PRNAS(e2, std::underlying_type_t<decltype(e2)>);
        PRNAS(e3, std::underlying_type_t<decltype(e3)>);
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
        PRNAS(e2, std::underlying_type_t<decltype(e2)>);
        PRNAS(e3, std::underlying_type_t<decltype(e3)>);
        END;

        using qqq = typename sib::TValue<TEnum>::underlying_type;

        DEF(sib::TValue<TEnum        >, ve1, { e_1 });
        DEF(sib::TValue<TEnumClass   >, ve2, { 100 });
        DEF(sib::TValue<TEnumClass123>, ve3, { 100 });
        PRN(e1);
        PRNAS(e2, std::underlying_type_t<decltype(e2)>);
        PRNAS(e3, std::underlying_type_t<decltype(e3)>);
        END;
    } {
        BEG;
        DEFA(auto, i, = 1, int);
        DEFA(sib::TValue, val, = 2, sib::TValue<int>);
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
        DEFA(sib::TValue, val, = 111, sib::TValue<int>);
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
        std::cout << "                                             sib::TPointer                                             \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        DEF(TFn*, ptrFoo, (foo));
        DEFA(sib::TPointer, PointerFoo, (foo), sib::TPointer<TFn>);
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
        DEFA(sib::TPointer, ptr, (&i), sib::TPointer<int>);
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
        DEFA(sib::TPointer, ptr, (&i), sib::TPointer<int const>);
        PRN(ptr);
        PRN(&i);
        END;
    } {
        BEG;
        DEF(sib::TPointer<int>, ptr, );
        PRN(ptr);
        END;
        DEF(int, i, = 10);
        EXE(ptr = &i);
        EXE(*ptr = 222222);
        PRN(i);
        END;
    } {
        BEG;
        DEF(sib::TPointer<int const>, ptr, );
        PRN(ptr);
        END;
        DEF(int, i, = 10);
        EXE(ptr = &i);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(sib::TPointer<int>, ptr, );
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
        DEFA(sib::TPointer, ptr, (&s), sib::TPointer<std::string>);
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
        DEFA(sib::TPointer, ptr, (&s1), sib::TPointer<std::string const>);
        PRN(ptr);
        END;
        EXE(ptr = &s2);
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(std::string, s, = "qwerty");
        DEF(sib::TPointer<std::string const> const, ptr, (&s));
        PRN(ptr);
        END;
        EXE(s = "!!!!!!!!");
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(int, i, = 111);
        DEFA(sib::TPointer const, cptr_i, = &i, sib::TPointer<int> const);
        DEF(sib::TPointer<int const>, ptr_ci, );
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
        DEFA(sib::TPointer, ptr, ("qwerty"), sib::TPointer<char const>);
        PRN(ptr);
        END;
        EXE(ptr = "!!!!!!!!!!!!!!");
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(sib::TPointer<std::nullptr_t>, ptr, );
        PRN(ptr);
        END;
    } {
        BEG;
        DEF(sib::TPointer<void>, ptr, (nullptr));
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
        EXE(ptr = ptr - 3);
        PRN(ptr);
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
        PRN(ptr2 - ptr1);
        END;
    }
#endif TEST_POINTER

#ifdef TEST_ARRAY
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                              sib::TArray                                              \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        DEF(int, i, = 222);
        DEF(int const, ic, = 111);
        DEFA(sib::TArray, arr, (i _ ic _ 333 _ ic _ i), sib::TArray<int _ 5>);
        PRN(arr);
        END;
    } {
        BEG;
        DEFA(sib::TArray, arr, (1 _ 2 _ 3), sib::TArray<int _ 3>);
        DEFA(sib::TArray, arrarr, (arr _ arr _ arr), sib::TArray<sib::TArray<int _ 3> _ 3>);
        PRN(arr);
        PRN(arrarr);
        END;
    } {
        BEG;
        DEFA(sib::TArray, arr, { 1 _ 2 _ 3 _ 4 _ 5 }, sib::TArray<int _ 5>);
        PRN(arr);
        END;
        EXE(arr = { 8 _ 7 _ 6 _ 5 _ 4 });
        PRN(arr);
        END;
    } {
        BEG;
        DEF(static constexpr char, cech, = '1');
        DEF(static constexpr char, cech0, = '\0');
        DEFA(sib::TArray, arr, { cech _ cech _ cech _ cech0 }, sib::TArray<char _ 4>);
        PRN(arr);
        END;
    } {
        BEG;
        DEFA(sib::TArray, arr, { '1' _ '2' _ '3' _ '4' _ '5' _ '\0' }, sib::TArray<char _ 6>);
        PRN(arr);
        END;
        EXE(arr = "87654");
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char, ch10, [10] = "12345");
        DEFA(sib::TArray, arr, = ch10, sib::TArray<char _ 10>);
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
        DEFA(sib::TArray, arr1, = "12345", sib::TArray<char _ 6>);
        DEFA(sib::TArray, arr2, = "54321", sib::TArray<char _ 6>);
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
        DEFA(sib::TArray, arr, = cha, sib::TArray<char _ 4>);
        PRN(cha);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char const, cch, = '1');
        DEF(char const, cch0, = '\0');
        DEFA(sib::TArray, arr, { cch _ cch _ cch _ cch0 }, sib::TArray<char _ 4>);
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
        DEFA(sib::TArray, arr, = "999", sib::TArray<char _ 4>);
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
        DEFA(sib::TArray, arr, = charr, sib::TArray<char _ 7>);
        PRN(charr);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(char, charr, [10] = "qwerty111");
        DEFA(sib::TArray, arr, = charr, sib::TArray<char _ 10>);
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
        DEFA(sib::TArray, arr, = starr, sib::TArray<int _ 4>);
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
        DEFA(sib::TArray, arr, = sarr, sib::TArray<char _ 4>);
        PRN(sarr);
        PRN(arr);
        END;
    } {
        BEG;
        DEF(sib::TArray, arr, (std::array{ 'f' _ 'f' _ 'f' _ 'f' }));
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
        PRNAS(TEnumClass123(tmp), std::underlying_type_t<TEnumClass123>);
        END;
        EXE(static_cast<int&>(tmp) = 444);
        EXE(static_cast<TEnum&>(tmp) = e_3);
        EXE(static_cast<TEnumClass123&>(tmp) = TEnumClass123::_1);
        PRN(tmp);
        PRN(int(tmp));
        PRN(TEnum(tmp));
        PRNAS(TEnumClass123(tmp), std::underlying_type_t<TEnumClass123>);
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
        DEF(auto, w, = sib::to_wrap(L"123456789ффф"s));
        PRN(w);
        END;
    } {
        BEG;
        DEFA(char, c, [] = "123", char[4]);
        END;
    } {
        BEG;
        DEF(std::nullptr_t, nptr, {});
        DEF(decltype(auto), w, = sib::to_wrap(nptr));
        PRN(nptr);
        PRN(w);
        END;
    } {
        BEG;
        DEF(std::nullptr_t const, nptr, {});
        DEF(decltype(auto), w, = sib::to_wrap(nptr));
        PRN(nptr);
        PRN(w);
        END;
    }
#endif TEST_WRAPPER

#ifdef TEST_TYPE_PACK
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                           type pack                                              \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        EXE(using f1  = sib::types_first_t<A _ B _ C>);
        EXE(using f2  = sib::types_first_t<C _ B _ B>);
        EXE(using f3  = sib::types_first_t<D _ D _ B>);
      //EXE(using f4  = sib::types_first_t<>);
        EXE(using f5  = sib::types_first_t<E>);
        EXE(using f6  = sib::types_first_t<sib::type_pack<A _ B _ C>>);
        EXE(using f7  = sib::types_first_t<sib::type_pack<C _ B _ B>>);
        EXE(using f8  = sib::types_first_t<sib::type_pack<D _ D _ B>>);
      //EXE(using f9  = sib::types_first_t<sib::type_pack<>>);
        EXE(using f10 = sib::types_first_t<sib::type_pack<E>>);
        EXE(using f11 = sib::types_first_t<sib::type_pack<A _ E> _ B _ C _ sib::type_pack<D _ A>>);
        EXE(using f12 = sib::types_first_t<sib::type_pack<sib::type_pack<D _ B>>>);
        EXE(using f13 = sib::types_first_t<sib::type_pack<sib::type_pack<A _ E> _ B _ C _ sib::type_pack<D _ A>>>);
        DEFA(f1 , v1 , , A);
        DEFA(f2 , v2 , , C);
        DEFA(f3 , v3 , , D);
      //DEF (f4 , v4 ,    );
        DEFA(f5 , v5 , , E);
        DEFA(f6 , v6 , , A);
        DEFA(f7 , v7 , , C);
        DEFA(f8 , v8 , , D);
      //DEF (f9 , v9 ,    );
        DEFA(f10, v10, , E);
        DEFA(f11, v11, , sib::type_pack<A _ E>);
        DEFA(f12, v12, , sib::type_pack<D _ B>);
        DEFA(f13, v13, , sib::type_pack<A _ E>);
        END;
    } {
        BEG;
        EXE(using f1  = sib::types_last_t<A _ B _ C>);
        EXE(using f2  = sib::types_last_t<C _ B _ B>);
        EXE(using f3  = sib::types_last_t<D _ D _ B>);
      //EXE(using f4  = sib::types_last_t<>);
        EXE(using f5  = sib::types_last_t<E>);
        EXE(using f6  = sib::types_last_t<sib::type_pack<A _ B _ C>>);
        EXE(using f7  = sib::types_last_t<sib::type_pack<C _ B _ B>>);
        EXE(using f8  = sib::types_last_t<sib::type_pack<D _ D _ B>>);
      //EXE(using f9  = sib::types_last_t<sib::type_pack<>>);
        EXE(using f10 = sib::types_last_t<sib::type_pack<E>>);
        EXE(using f11 = sib::types_last_t<sib::type_pack<A _ E> _ B _ C _ sib::type_pack<D _ A>>);
        EXE(using f12 = sib::types_last_t<sib::type_pack<sib::type_pack<D _ B>>>);
        EXE(using f13 = sib::types_last_t<sib::type_pack<sib::type_pack<A _ E> _ B _ C _ sib::type_pack<D _ A>>>);
        DEFA(f1 , v1 , , C);
        DEFA(f2 , v2 , , B);
        DEFA(f3 , v3 , , B);
      //DEF (f4 , v4 ,    );
        DEFA(f5 , v5 , , E);
        DEFA(f6 , v6 , , C);
        DEFA(f7 , v7 , , B);
        DEFA(f8 , v8 , , B);
      //DEF (f9 , v9 ,    );
        DEFA(f10, v10, , E);
        DEFA(f11, v11, , sib::type_pack<D _ A>);
        DEFA(f12, v12, , sib::type_pack<D _ B>);
        DEFA(f13, v13, , sib::type_pack<D _ A>);
        END;
    } {
        static constexpr int _C = 10;
        static constexpr int _I = 7;
        BEG;
        EXE(using Ts = gen_TP<_C>);
        PRN(sib::static_type_name<Ts>());
        std::cout << "    length     = " << sib::static_type_name<Ts>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<Ts>::size << "\n";
        PRN(Types_to_Str(Ts{}));
        END;
        EXE(using H = sib::types_head<_I _ Ts>);
        PRN(sib::static_type_name<H>());
        std::cout << "    length     = " << sib::static_type_name<H>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<H>::size << "\n";
        PRN(Types_to_Str(H{}));
        END;
        EXE(using T = sib::types_tail<_I _ Ts>);
        PRN(sib::static_type_name<T>());
        std::cout << "    length     = " << sib::static_type_name<T>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<T>::size << "\n";
        PRN(Types_to_Str(T{}));
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_tail<10, gen_TP<30>>);
        PRN(sib::static_type_name<Ts>());
        std::cout << "    length     = " << sib::static_type_name<Ts>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<Ts>::size << "\n";
        PRN(Types_to_Str(Ts{}));
        END;
        EXE(using STs = sib::types_sorted_t<Ts>);
        PRN(sib::static_type_name<STs>());
        std::cout << "    length     = " << sib::static_type_name<STs>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<STs>::size << "\n";
        PRN(Types_to_Str(STs{}));
        END;
        EXE(static_assert(sib::is_sorted_v<STs>));
        END;
    } {
        BEG;
        EXE(using STs = sib::types_sorted_t<sib::type_pack<>>);
        PRN(sib::static_type_name<STs>());
        std::cout << "    length     = " << sib::static_type_name<STs>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<STs>::size << "\n";
        PRN(Types_to_Str(STs{}));
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_summ_t<gen_TP<5>, gen_TP<3>>);
        std::cout << "    length     = " << sib::static_type_name<Ts>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<Ts>::size << "\n";
        PRN(Types_to_Str(Ts{}));
        END;
        EXE(using STs = sib::types_sorted_t<Ts>);
        PRN(sib::static_type_name<STs>());
        std::cout << "    length     = " << sib::static_type_name<STs>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<STs>::size << "\n";
        PRN(Types_to_Str(STs{}));
        END;
    } {
        BEG;
        EXE(using Ts = sib::type_pack<sib::type_pack<>, sib::type_pack<>, int, sib::type_pack<>, int, float, sib::type_pack<>, int, sib::type_pack<>>);
        std::cout << "    length     = " << sib::static_type_name<Ts>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<Ts>::size << "\n";
        PRN(Ts{});
        END;
        EXE(using STs = sib::types_sorted_t<Ts>);
        PRN(sib::static_type_name<STs>());
        std::cout << "    length     = " << sib::static_type_name<STs>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<STs>::size << "\n";
        PRN(STs{});
        END;
    }
#endif TEST_TYPE_PACK

#ifdef TEST_TYPE_LIST
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                           type list                                              \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        EXE(using f1  = sib::types_first_t<A _ B _ C>);
        EXE(using f2  = sib::types_first_t<C _ B _ B>);
        EXE(using f3  = sib::types_first_t<D _ D _ B>);
      //EXE(using f4  = sib::types_first_t<>);
        EXE(using f5  = sib::types_first_t<E>);
        EXE(using f6  = sib::types_first_t<sib::type_list<A _ B _ C>>);
        EXE(using f7  = sib::types_first_t<sib::type_list<C _ B _ B>>);
        EXE(using f8  = sib::types_first_t<sib::type_list<D _ D _ B>>);
      //EXE(using f9  = sib::types_first_t<sib::type_list<>>);
        EXE(using f10 = sib::types_first_t<sib::type_list<E>>);
        EXE(using f11 = sib::types_first_t<sib::type_list<A _ E> _ B _ C _ sib::type_list<D _ A>>);
        EXE(using f12 = sib::types_first_t<sib::type_list<sib::type_list<D _ B>>>);
        EXE(using f13 = sib::types_first_t<sib::type_list<sib::type_list<A _ E> _ B _ C _ sib::type_list<D _ A>>>);
        DEFA(f1 , v1 , , A);
        DEFA(f2 , v2 , , C);
        DEFA(f3 , v3 , , D);
      //DEF (f4 , v4 ,    );
        DEFA(f5 , v5 , , E);
        DEFA(f6 , v6 , , A);
        DEFA(f7 , v7 , , C);
        DEFA(f8 , v8 , , D);
      //DEF (f9 , v9 ,    );
        DEFA(f10, v10, , E);
        DEFA(f11, v11, , sib::type_list<A _ E>);
        DEFA(f12, v12, , sib::type_list<D _ B>);
        DEFA(f13, v13, , sib::type_list<A _ E>);
        END;
    } {
        BEG;
        EXE(using f1  = sib::types_last_t<A _ B _ C>);
        EXE(using f2  = sib::types_last_t<C _ B _ B>);
        EXE(using f3  = sib::types_last_t<D _ D _ B>);
      //EXE(using f4  = sib::types_last_t<>);
        EXE(using f5  = sib::types_last_t<E>);
        EXE(using f6  = sib::types_last_t<sib::type_list<A _ B _ C>>);
        EXE(using f7  = sib::types_last_t<sib::type_list<C _ B _ B>>);
        EXE(using f8  = sib::types_last_t<sib::type_list<D _ D _ B>>);
      //EXE(using f9  = sib::types_last_t<sib::type_list<>>);
        EXE(using f10 = sib::types_last_t<sib::type_list<E>>);
        EXE(using f11 = sib::types_last_t<sib::type_list<A _ E> _ B _ C _ sib::type_list<D _ A>>);
        EXE(using f12 = sib::types_last_t<sib::type_list<sib::type_list<D _ B>>>);
        EXE(using f13 = sib::types_last_t<sib::type_list<sib::type_list<A _ E> _ B _ C _ sib::type_list<D _ A>>>);
        DEFA(f1 , v1 , , C);
        DEFA(f2 , v2 , , B);
        DEFA(f3 , v3 , , B);
      //DEF (f4 , v4 ,    );
        DEFA(f5 , v5 , , E);
        DEFA(f6 , v6 , , C);
        DEFA(f7 , v7 , , B);
        DEFA(f8 , v8 , , B);
      //DEF (f9 , v9 ,    );
        DEFA(f10, v10, , E);
        DEFA(f11, v11, , sib::type_list<D _ A>);
        DEFA(f12, v12, , sib::type_list<D _ B>);
        DEFA(f13, v13, , sib::type_list<D _ A>);
        END;
    } {
        static constexpr int _C = 10;
        static constexpr int _I = 7;
        BEG;
        EXE(using Ts = gen_TL<_C>);
        PRN(sib::static_type_name<Ts>());
        std::cout << "    length     = " << sib::static_type_name<Ts>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<Ts>::size << "\n";
        PRN(Types_to_Str(Ts{}));
        END;
        EXE(using H = sib::types_head<_I _ Ts>);
        PRN(sib::static_type_name<H>());
        std::cout << "    length     = " << sib::static_type_name<H>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<H>::size << "\n";
        PRN(Types_to_Str(H{}));
        END;
        EXE(using T = sib::types_tail<_I _ Ts>);
        PRN(sib::static_type_name<T>());
        std::cout << "    length     = " << sib::static_type_name<T>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<T>::size << "\n";
        PRN(Types_to_Str(T{}));
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_tail<10, gen_TL<30>>);
        std::cout << "    length     = " << sib::static_type_name<Ts>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<Ts>::size << "\n";
        PRN(Types_to_Str(Ts{}));
        END;
        EXE(using STs = sib::types_sequence_t<Ts>);
        PRN(sib::static_type_name<STs>());
        std::cout << "    length     = " << sib::static_type_name<STs>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<STs>::size << "\n";
        PRN(Types_to_Str(STs{}));
        END;
        EXE(static_assert(sib::is_sorted_v<STs>));
        END;
    } {
        BEG;
        EXE(using STs = sib::types_sequence_t<sib::type_list<>>);
        PRN(sib::static_type_name<STs>());
        std::cout << "    length     = " << sib::static_type_name<STs>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<STs>::size << "\n";
        PRN(Types_to_Str(STs{}));
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_summ_t<gen_TL<5>, gen_TL<3>>);
        std::cout << "    length     = " << sib::static_type_name<Ts>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<Ts>::size << "\n";
        PRN(Types_to_Str(Ts{}));
        END;
        EXE(using STs = sib::types_sequence_t<Ts>);
        PRN(sib::static_type_name<STs>());
        std::cout << "    length     = " << sib::static_type_name<STs>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<STs>::size << "\n";
        PRN(Types_to_Str(STs{}));
        END;
    } {
        BEG;
        EXE(using Ts = sib::type_list<sib::type_list<>, sib::type_list<>, int, sib::type_list<>, int, float, sib::type_list<>, int, sib::type_list<>>);
        std::cout << "    length     = " << sib::static_type_name<Ts>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<Ts>::size << "\n";
        PRN(Ts{});
        END;
        EXE(using STs = sib::types_sequence_t<Ts>);
        PRN(sib::static_type_name<STs>());
        std::cout << "    length     = " << sib::static_type_name<STs>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<STs>::size << "\n";
        PRN(STs{});
        END;
    }
#endif TEST_TYPE_LIST

#ifdef TEST_UNIQUE_TUPLE
    {
        std::cout << "\n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "                                           TUniqueTuple                                           \n";
        std::cout << "**************************************************************************************************\n";
        std::cout << "\n";
    } {
        BEG;
        EXE(
            static_assert(std::is_same_v<
                sib::MakeUniqueTuple<std::vector<int>, bool, std::string>,
                sib::MakeUniqueTuple<std::string, std::vector<int, std::allocator<int>>, bool>
            >)
        );
        EXE(
            static_assert(std::is_same_v<
                sib::MakeUniqueTuple<A, B, C>,
                sib::MakeUniqueTuple<B, C, A>
            >)
        );

        DEF(sib::MakeUniqueTuple<A _ B _ C>, tmp1, );
        DEF(sib::MakeUniqueTuple<C _ B _ A>, tmp2, );
        DEF(sib::MakeUniqueTuple<B _ A _ C>, tmp3, );

        DEF(sib::MakeUniqueTuple<std::vector<int> _ bool _ std::string>, tmp4, );
        DEF(sib::MakeUniqueTuple<std::string _ std::vector<int _ std::allocator<int>> _ bool>, tmp5, );

        EXE(static_assert(std::is_same_v<decltype(tmp1), decltype(tmp2)>));
        EXE(static_assert(std::is_same_v<decltype(tmp2), decltype(tmp3)>));
        EXE(static_assert(std::is_same_v<decltype(tmp3), decltype(tmp1)>));

        EXE(static_assert(std::is_same_v<decltype(tmp4), decltype(tmp5)>));
        EXE(static_assert(std::is_same_v<decltype(tmp5), decltype(tmp4)>));

        PRN(tmp1);
        PRN(tmp2);
        PRN(tmp3);
        PRN(tmp4);
        PRN(tmp5);

        PRN(sib::is_container_v<decltype(tmp4)>);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<int>, tmp, );
        EXE(tmp = 111);
        PRN(tmp);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<std::string>, tmp, );
        EXE(tmp = "xxxxxxxxx");
        PRN(sib::is_container_v<decltype(tmp)>);
        PRN(tmp);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<int _ std::string>, tmp, );
        EXE(tmp = 111);
        EXE(tmp = "xxxxxxxxx");
        PRN(sib::is_container_v<decltype(tmp)>);
        PRN(tmp);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<int _ std::string _ std::vector<int>>, tmp, );
        EXE(tmp.as<std::vector<int>>() = std::vector<int>{1 _ 2 _ 3});
        EXE(tmp = 2.2);
        EXE(tmp = "xxxxxxxxx");
        PRN(sib::is_container_v<decltype(tmp)>);
        PRN(tmp.as<std::vector<int>>());
        PRN(tmp.as<int>());
        PRN(tmp.as<std::string>());
        PRN(tmp);
        END;
        EXE(tmp.as<int>() = 3.3);
        PRN(tmp.as<int>());
        END;
        DEF(float, f, = tmp);
        PRN(f);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<std::string>, ut, = "qwerty");
        PRN(ut);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<int _ std::string>, ut, { "qwerty" _ 111 });
        DEF(float, f, = ut);
        DEF(std::string, s, = ut);
        PRN(f);
        PRN(s);
        PRN(ut);
        PRN(foo(ut));
        PRN(bar(ut));
        PRN(baz(ut));
        END;
    } {
        BEG;
        DEF(auto, ut, = sib::make_unique_tuple("qwerty"s _ 111));
        DEF(float, f, = ut);
        DEF(std::string, s, = ut);
        PRN(f);
        PRN(s);
        PRN(ut);
        PRN(foo(ut));
        PRN(bar(ut));
        PRN(baz(ut));
        END;
    } {
        BEG;
        EXE(using UT1 = sib::MakeUniqueTuple<gen_TP<10>>);
        EXE(UT1 ut1{});
        std::cout << "    length     = " << sib::static_type_name<UT1>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<UT1::types<sib::type_pack>>::size << "\n";
        PRN(Types_to_Str(UT1::types<sib::type_pack>{}));
        END;
        EXE(using TL = decltype(gen_TL<20>{}.get_tail<10>()));
        EXE(using UT2 = sib::MakeUniqueTuple<TL>);
        EXE(UT2 ut2{});
        std::cout << "    length     = " << sib::static_type_name<UT2>().size() << "\n";
        std::cout << "    type count = " << sib::types_info<UT2::types<sib::type_list>>::size << "\n";
        PRN(Types_to_Str(UT2::types<sib::type_list>{}));
        END;
        PRN(std::is_same_v<UT1 _ UT2>);
        END;
    } {
        BEG;
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
