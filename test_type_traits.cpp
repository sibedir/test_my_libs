#include "test_type_traits.h"

#include <memory>
#include <type_traits>
#include <string>
#include <vector>
#include <set>
#include <iostream>

#include "sib_support.h"
#include "sib_unit_test.h"
#include "sib_type_traits.h"

#include "sib_wrapper.h"

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
class TPointer2
{
public:
    using base_type = T;
    using data_type = sib::TPointer<base_type>;
private:
    data_type data;
public:
    constexpr TPointer2() = default;
    constexpr TPointer2(TPointer2 const &) = default;
    constexpr TPointer2(TPointer2      &&) = default;
    constexpr TPointer2& operator=(TPointer2 const &) = default;
    constexpr TPointer2& operator=(TPointer2      &&) = default;
    constexpr ~TPointer2() = default;

    constexpr TPointer2(data_type const ptr) noexcept : data(ptr) {}

    constexpr operator data_type const & () const noexcept { return data; }
    constexpr operator data_type       & ()       noexcept { return data; }

    constexpr data_type& operator = (data_type const & other) noexcept { return data = other; }

    constexpr base_type& operator * () const noexcept { return *data; }
    constexpr data_type  operator ->() const noexcept { return  data; }

    constexpr data_type& operator ++ () noexcept { return ++data; }
    constexpr data_type& operator -- () noexcept { return --data; }

    constexpr data_type operator ++ (int) noexcept { return data++; }
    constexpr data_type operator -- (int) noexcept { return data--; }

    constexpr operator bool () const noexcept { return data; }
};

// ---------------------------------------------------------------------------------------------------------------------

using TFn = int(double);

// ---------------------------------------------------------------------------------------------------------------------

struct None {};

template <typename> struct deref { using type = None; };
template <typename> struct arrow { using type = None; };
template <typename> struct contr { using type = None; };

template <typename T> using deref_t = typename deref <T>::type;
template <typename T> using arrow_t = typename arrow <T>::type;
template <typename T> using contr_t = typename contr <T>::type;

template <sib::Dereferenceable T>
struct deref<T>
{
  using type = sib::base_of_indirect_type<T>;
};

template <typename T> requires(sib::has_arrow_v<T>)
struct arrow<T>
{
  using type = sib::arrow_result_t<T>;
};

template <typename T> requires(sib::Dereferenceable<sib::arrow_result_t<T>>)
struct contr<T>
{
  using type = decltype(*std::declval<sib::arrow_result_t<T>>());
};

// TEST MACRO
#define TM(ptr, def, arw, lptr, fn, lfn, arr, cont, ...)\
namespace SIB_CONCAT(NS, __COUNTER__) {                 \
    using T = __VA_ARGS__;                              \
    static_assert(ptr  std::is_pointer_v        <T>);   \
    static_assert(def  sib::is_dereferenceable_v<T>);   \
    static_assert(arw  sib::has_arrow_v         <T>);   \
    static_assert(lptr sib::is_like_pointer_v   <T>);   \
    static_assert(                                      \
        not sib::LikePointer<T>                         \
        or (                                            \
                sib::Pointer<T>                         \
            or  sib::LikeFunction<T>                    \
            or  sib::Same<deref_t<T>, contr_t<T>>       \
        )                                               \
    );                                                  \
    static_assert(fn   sib::is_function_v       <T>);   \
    static_assert(lfn  sib::is_like_function_v  <T>);   \
    static_assert(arr  sib::is_array_v          <T>);   \
    static_assert(cont sib::is_container_v      <T>);   \
}

//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|--------------------------------------------|
//|  is ptr   |   deref   | has arrow | like ptr  |  is func  | like func |  is arr   | container |                    type                    |
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|--------------------------------------------|
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , void                                       )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , int                                        )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , sib::TWrapper<int>                         )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|--------------------------------------------|
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , std::nullptr_t                             )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , sib::TNullPtr                              )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|--------------------------------------------|
TM(           ,    not    ,    not    ,           ,    not    ,    not    ,    not    ,    not    , void*                                      )
TM(           ,           ,    not    ,           ,    not    ,    not    ,    not    ,    not    , int*                                       )
TM(    not    ,    not    ,           ,           ,    not    ,    not    ,    not    ,    not    , sib::TPointer<void>                        )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , sib::TPointer<int>                         )
TM(    not    ,    not    ,           ,           ,    not    ,    not    ,    not    ,    not    , sib::TWrapper<void*>                       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , sib::TWrapper<int*>                        )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|--------------------------------------------|
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , TPointer2<int>                             )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , std::shared_ptr<int>                       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , std::unique_ptr<int>                       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , sib::TPointer<std::unique_ptr<int*>>       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , sib::TPointer<void*>                       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , std::vector<int>::iterator                 )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , std::set<int>::iterator                    )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , std::vector<sib::TPointer<void>>::iterator )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , std::vector<TPointer2<int>>::iterator      )
TM(           ,           ,    not    ,           ,    not    ,           ,    not    ,    not    , TFn*                                       )
TM(    not    ,           ,    not    ,           ,    not    ,           ,    not    ,    not    , sib::TPointer<TFn>                         )
TM(    not    ,           ,    not    ,           ,    not    ,           ,    not    ,    not    , sib::TWrapper<TFn*>                        )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|--------------------------------------------|
TM(    not    ,    not    ,    not    ,    not    ,           ,           ,    not    ,    not    , TFn                                        )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,           ,    not    ,    not    , std::function<TFn>                         )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,           ,    not    ,    not    , sib::TWrapper<TFn>                         )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|--------------------------------------------|
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           ,           , int[5]                                     )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           , sib::TArray<int, 5>                        )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           , std::array<int, 5>                         )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           , std::vector<int>                           )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           , std::string                                )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|--------------------------------------------|

#undef TM

// ---------------------------------------------------------------------------------------------------------------------

struct MyClass
{
    MyClass()                { sib::debug::outstream << "MyClass()\n"               ; }
    MyClass(MyClass const &) { sib::debug::outstream << "MyClass(MyClass const &)\n"; }
    MyClass(MyClass      &&) { sib::debug::outstream << "MyClass(MyClass &&)\n"     ; }

    template <typename T>
    MyClass(T) { sib::debug::outstream << "MyClass(" << sib::type_name<T>() << ")\n"; }

    virtual ~MyClass() { sib::debug::outstream << "~MyClass()\n"; }
    virtual void aaa() {}
    operator char const* () const { return "MyClass"; }
};

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_type_traits)
{
    sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                          sib_type_traits                                           ");
    MSG("****************************************************************************************************");
    MSG("");

    #define TM(lp, ...)                                             \
    {                                                               \
        BEG;                                                        \
        MSG(#__VA_ARGS__);                                          \
        using T = __VA_ARGS__;                                      \
        if constexpr (std::is_default_constructible_v<T>)           \
            [[maybe_unused]] T tmp {};                              \
        ASSERT(lp sib::is_like_pointer_v<__VA_ARGS__>);             \
        TYP(deref_t<T>);                                            \
        TYP(arrow_t<T>);                                            \
        END;                                                        \
    }
    
    TM( not , int                                        );
    TM( not , sib::TWrapper<int>                         );
    TM( not , std::nullptr_t                             );
    TM( not , sib::TNullPtr                              );
    TM(     , void*                                      );
    TM(     , int*                                       );
    TM(     , sib::TPointer<void>                        );
    TM(     , sib::TPointer<int>                         );
    TM(     , sib::TWrapper<void*>                       );
    TM(     , sib::TWrapper<int*>                        );
    TM(     , TPointer2<int>                             );
    TM(     , std::shared_ptr<int>                       );
    TM(     , std::unique_ptr<int>                       );
    TM(     , sib::TPointer<std::unique_ptr<int*>>       );
    TM(     , sib::TPointer<void*>                       );
    TM(     , std::vector<int>::iterator                 );
    TM(     , std::set<int>::iterator                    );
    TM(     , std::vector<sib::TPointer<void>>::iterator );
    TM(     , std::vector<TPointer2<int>>::iterator      );
    TM(     , TFn*                                       );
    TM(     , sib::TPointer<TFn>                         );
    TM(     , sib::TWrapper<TFn*>                        );
    TM( not , std::function<TFn>                         );
    TM( not , sib::TWrapper<TFn>                         );
    TM( not , int[5]                                     );
    TM( not , sib::TArray<int, 5>                        );
    TM( not , std::array<int, 5>                         );
    TM( not , std::vector<int>                           );
    TM( not , std::string                                );
    
    #undef TM

    {
        BEG;
        ASSERT( sib::is_convertible_from_tooneof_v<int, float, std::string>);
        PRN   ( sib::convert_from_tooneof_select  <int, float, std::string>{});
        ASSERT(!sib::is_convertible_from_tooneof_v<int, float, char, std::string>);
        ASSERT(!sib::is_convertible_from_tooneof_v<int, std::string, std::vector<int>>);
        ASSERT( sib::is_convertible_from_tooneof_v<int, std::string, sib::TWrapper<float>, std::vector<int>>);
        PRN   ( sib::convert_from_tooneof_select  <int, std::string, sib::TWrapper<float>, std::vector<int>>{});
        ASSERT(!sib::is_convertible_from_tooneof_v<int, std::string, sib::TWrapper<float>, std::vector<int>, sib::TWrapper<int>>);
        END;
    } {
        struct C1 {};
        struct C2 { // -> int
            int i;
            operator int const & () const { return i; }
            operator int& () { return i; }
        };
        struct C3 { // -> C2
            C2 c2;
            C3() {};
            C3(int const &) {};
            operator C2& () { return c2; }
        };
        struct C4 { // -> C2
            C2 c2;
            C4() = default;
            C4(C4 const&) = default;
            C4(C4 &&) = default;
            C4(int const &) {};
            C4& operator= (C4 const& other) = default;
        };

        BEG;
        ASSERT(!std::is_convertible_v<C2, C4>);
        ASSERT( std::is_constructible_v<C4, C2>);
        ASSERT(!sib::is_convertible_from_to_v<C2, C4>);
        ASSERT( sib::is_constructible_to_from_v<C4, C2>);
        END;
        ASSERT(!sib::is_convertible_from_tooneof_v<C1, C2, C3>);
        ASSERT( sib::is_convertible_from_tooneof_v<C3, C1, C2, C1>);
        PRN   ( sib::convert_from_tooneof_select  <C3, C1, C2, C1>{});
        ASSERT( sib::is_convertible_from_tooneof_v<C3, C2, C1, int>);
        PRN   ( sib::convert_from_tooneof_select  <C3, C2, C1, int>{});
        ASSERT( sib::is_convertible_from_tooneof_v<C2, C2, C1, C3>);
        PRN   ( sib::convert_from_tooneof_select  <C2, C2, C1, C3>{});
        ASSERT( sib::is_convertible_from_tooneof_v<C2, C1, int, C3>);
        PRN   ( sib::convert_from_tooneof_select  <C2, C1, int, C3>{});
        END;
        ASSERT(!sib::is_convertible_to_fromoneof_v<C1, C2, C3>);
        ASSERT( sib::is_convertible_to_fromoneof_v<int, C1, C2, C3>);
        PRN   ( sib::convert_to_fromoneof_select  <int, C1, C2, C3>{});
        ASSERT( sib::is_convertible_to_fromoneof_v<C3, C2, C1, int>);
        ASSERT( sib::is_convertible_to_fromoneof_v<C3, C1, int, MyClass>);
        PRN   ( sib::convert_to_fromoneof_select  <C3, C1, int, MyClass>{});
        ASSERT(!sib::is_convertible_to_fromoneof_v<C2, C2, C1, C3>);
        ASSERT( sib::is_convertible_to_fromoneof_v<C2, C2>);
        ASSERT(!sib::is_convertible_to_fromoneof_v<C2, C1>);
        ASSERT( sib::is_convertible_to_fromoneof_v<C2, C3>);
        ASSERT( sib::is_convertible_to_fromoneof_v<C2, C1, int, C3>);
        PRN   ( sib::convert_to_fromoneof_select  <C2, C1, int, C3>{});
        END;
    } {
        struct C1 {}; // <- C1, C4
        struct C2 {}; // <- C2
        struct C3 {   // <- C2, C3
            C3() {}
            C3(C2 const&) {}
        };
        struct C4 : C1 { // <- C2, C4
            C4() {}
            C4(C2 const&) {}
        };
        struct C5 { // <- C2, C4
            C5() {}
            C5(C3 const&) {}
        };

        BEG;
        ASSERT(!sib::is_constructible_from_tooneof_v<C1, C2, C4, C3>);
        ASSERT( sib::is_constructible_from_tooneof_v<C1, C2, C1, C3>);
        PRN   ( sib::construct_from_tooneof_select  <C1, C2, C1, C3>{});
        ASSERT( sib::is_constructible_from_tooneof_v<C4, C2, C1, C3>);
        PRN   ( sib::construct_from_tooneof_select  <C4, C2, C1, C3>{});
        ASSERT(!sib::is_constructible_from_tooneof_v<C4, C2, C1, C4>);
        END;
        ASSERT(!sib::is_constructible_to_fromoneof_v<C5, C1, C2, C3>);
        ASSERT( sib::is_constructible_to_fromoneof_v<C1, C2, C4, C3>);
        PRN   ( sib::construct_to_fromoneof_select  <C1, C2, C4, C3>{});
        ASSERT( sib::is_constructible_to_fromoneof_v<C1, C2, C1, C3>);
        PRN   ( sib::construct_to_fromoneof_select  <C1, C2, C1, C3>{});
        ASSERT(!sib::is_constructible_to_fromoneof_v<C1, C2, C4, C1, C3>);
        END;
    }

    sib::debug::outstream << std::endl;
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
std::string prn_type() { return std::string(sib::static_type_name<T>()); }

template <>
std::string prn_type<None>() { return "(x)"; }

// ---------------------------------------------------------------------------------------------------------------------

template <std::size_t... idx_>
consteval auto gen_TP_impl(std::index_sequence<idx_...>)
{
    return sib::types_pack< sib::int_tag<idx_> ... > {};
}

template <size_t... idx_>
consteval auto gen_TL_impl(std::index_sequence<idx_...>)
{
    return sib::types_list< sib::int_tag<idx_> ... > {};
}

template <size_t N> using gen_TP = decltype(gen_TP_impl(std::make_index_sequence<N>{}));
template <size_t N> using gen_TL = decltype(gen_TL_impl(std::make_index_sequence<N>{}));

template <typename T> struct Types_to_Str_Helper;

template <template <typename...> typename Tmpl, typename... Ts>
struct Types_to_Str_Helper<Tmpl<Ts...>>
{
    operator std::string() const
    {
        if constexpr (sizeof...(Ts) == 0) {
            return "<>";
        } else if constexpr (sizeof...(Ts) == 1) {
            return "<" + std::to_string(sib::types_first_t<Ts...>::value) + ">";
        } else {
            std::string res = "<";
            ((res += std::to_string(Ts::value) + ", "), ...);
            res[res.size() - 2] = '>';
            return res;
        }
    }
};

template <typename T>
std::string Types_to_Str() { return Types_to_Str_Helper<T>(); }

// ---------------------------------------------------------------------------------------------------------------------

struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

// ---------------------------------------------------------------------------------------------------------------------

#define _ ,

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_types_pack)
{
    sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                             types pack                                             ");
    MSG("****************************************************************************************************");
    MSG("");
    
    {
        BEG;
        EXE(using f1  = sib::types_first_t<A, B, C>);
        EXE(using f2  = sib::types_first_t<C, B, B>);
        EXE(using f3  = sib::types_first_t<D, D, B>);
      //EXE(using f4  = sib::types_first_t<>);
        EXE(using f5  = sib::types_first_t<E>);
        EXE(using f6  = sib::types_first_t<sib::types_pack<A, B, C>>);
        EXE(using f7  = sib::types_first_t<sib::types_pack<C, B, B>>);
        EXE(using f8  = sib::types_first_t<sib::types_pack<D, D, B>>);
      //EXE(using f9  = sib::types_first_t<sib::types_pack<>>);
        EXE(using f10 = sib::types_first_t<sib::types_pack<E>>);
        EXE(using f11 = sib::types_first_t<sib::types_pack<A, E>, B, C, sib::types_pack<D, A>>);
        EXE(using f12 = sib::types_first_t<sib::types_pack<sib::types_pack<D, B>>>);
        EXE(using f13 = sib::types_first_t<sib::types_pack<sib::types_pack<A, E>, B, C, sib::types_pack<D, A>>>);
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
        DEFA(f11, v11, , sib::types_pack<A _ E>);
        DEFA(f12, v12, , sib::types_pack<D _ B>);
        DEFA(f13, v13, , sib::types_pack<A _ E>);
        END;
    } {
        BEG;
        EXE(using f1  = sib::types_last_t<A, B, C>);
        EXE(using f2  = sib::types_last_t<C, B, B>);
        EXE(using f3  = sib::types_last_t<D, D, B>);
      //EXE(using f4  = sib::types_last_t<>);
        EXE(using f5  = sib::types_last_t<E>);
        EXE(using f6  = sib::types_last_t<sib::types_pack<A, B, C>>);
        EXE(using f7  = sib::types_last_t<sib::types_pack<C, B, B>>);
        EXE(using f8  = sib::types_last_t<sib::types_pack<D, D, B>>);
      //EXE(using f9  = sib::types_last_t<sib::types_pack<>>);
        EXE(using f10 = sib::types_last_t<sib::types_pack<E>>);
        EXE(using f11 = sib::types_last_t<sib::types_pack<A _ E> _ B _ C _ sib::types_pack<D _ A>>);
        EXE(using f12 = sib::types_last_t<sib::types_pack<sib::types_pack<D _ B>>>);
        EXE(using f13 = sib::types_last_t<sib::types_pack<sib::types_pack<A _ E> _ B _ C _ sib::types_pack<D _ A>>>);
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
        DEFA(f11, v11, , sib::types_pack<D _ A>);
        DEFA(f12, v12, , sib::types_pack<D _ B>);
        DEFA(f13, v13, , sib::types_pack<D _ A>);
        END;
    } {
        static constexpr int _C = 10;
        static constexpr int _I = 7;
        BEG;
        EXE(using Ts = gen_TP<_C>);
        MSG("    |", Types_to_Str<Ts>());
        MSG("    |type count = ", sib::types_info<Ts>::count);
        END;
        EXE(using H = sib::types_head_t<_I, Ts>);
        MSG("    |", Types_to_Str<H>());
        MSG("    |type count = ", sib::types_info<H>::count);
        END;

        EXE(using T = sib::types_tail_t<_I _ Ts>);
        MSG("    |", Types_to_Str<T>());
        MSG("    |type count = ", sib::types_info<T>::count);
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_tail_t<10, gen_TP<30>>);
        MSG("    |", Types_to_Str<Ts>());
        MSG("    |type count = ", sib::types_info<Ts>::count);
        END;

        EXE(using STs = sib::types_quick_sort_t<Ts>);
        MSG("    |", Types_to_Str<STs>());
        MSG("    |type count = ", sib::types_info<STs>::count);
        END;
    } {
        BEG;
        EXE(using STs = sib::types_quick_sort_t<sib::types_pack<>>);
        MSG("    |", sib::static_type_name<STs>());
        MSG("    |type count = ", sib::types_info<STs>::count);
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_summ_t<gen_TP<5>, gen_TP<3>>);
        MSG("    |", Types_to_Str<Ts>());
        MSG("    |type count = ", sib::types_info<Ts>::count);
        END;

        EXE(using STs = sib::types_quick_sort_t<Ts>);
        MSG("    |", Types_to_Str<STs>());
        MSG("    |type count = ", sib::types_info<STs>::count);
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_pack<sib::types_pack<>, sib::types_pack<>, int, sib::types_pack<>, int, float, sib::types_pack<>, int, sib::types_pack<>>);
        MSG("    |", sib::static_type_name<Ts>());
        MSG("    |type count = ", sib::types_info<Ts>::count);
        END;

        EXE(using STs = sib::types_quick_sort_t<Ts>);
        MSG("    |", sib::static_type_name<STs>());
        MSG("    |type count = ", sib::types_info<STs>::count);
        END;
    }

    sib::debug::outstream << std::endl;
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_types_list)
{
    sib::debug::Init();

    MSG("");                                              //
    MSG("****************************************************************************************************");
    MSG("                                             types list                                             ");
    MSG("****************************************************************************************************");
    MSG("");
    
    {
        BEG;
        EXE(using f1  = sib::types_first_t<A, B, C>);
        EXE(using f2  = sib::types_first_t<C, B, B>);
        EXE(using f3  = sib::types_first_t<D, D, B>);
      //EXE(using f4  = sib::types_first_t<>);
        EXE(using f5  = sib::types_first_t<E>);
        EXE(using f6  = sib::types_first_t<sib::types_list<A, B, C>>);
        EXE(using f7  = sib::types_first_t<sib::types_list<C, B, B>>);
        EXE(using f8  = sib::types_first_t<sib::types_list<D, D, B>>);
      //EXE(using f9  = sib::types_first_t<sib::types_list<>>);
        EXE(using f10 = sib::types_first_t<sib::types_list<E>>);
        EXE(using f11 = sib::types_first_t<sib::types_list<A, E>, B, C, sib::types_list<D, A>>);
        EXE(using f12 = sib::types_first_t<sib::types_list<sib::types_list<D, B>>>);
        EXE(using f13 = sib::types_first_t<sib::types_list<sib::types_list<A, E>, B, C, sib::types_list<D, A>>>);
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
        DEFA(f11, v11, , sib::types_list<A _ E>);
        DEFA(f12, v12, , sib::types_list<D _ B>);
        DEFA(f13, v13, , sib::types_list<A _ E>);
        END;
    } {
        BEG;
        EXE(using f1  = sib::types_last_t<A, B, C>);
        EXE(using f2  = sib::types_last_t<C, B, B>);
        EXE(using f3  = sib::types_last_t<D, D, B>);
      //EXE(using f4  = sib::types_last_t<>);
        EXE(using f5  = sib::types_last_t<E>);
        EXE(using f6  = sib::types_last_t<sib::types_list<A, B, C>>);
        EXE(using f7  = sib::types_last_t<sib::types_list<C, B, B>>);
        EXE(using f8  = sib::types_last_t<sib::types_list<D, D, B>>);
      //EXE(using f9  = sib::types_last_t<sib::types_list<>>);
        EXE(using f10 = sib::types_last_t<sib::types_list<E>>);
        EXE(using f11 = sib::types_last_t<sib::types_list<A _ E> _ B _ C _ sib::types_list<D _ A>>);
        EXE(using f12 = sib::types_last_t<sib::types_list<sib::types_list<D _ B>>>);
        EXE(using f13 = sib::types_last_t<sib::types_list<sib::types_list<A _ E> _ B _ C _ sib::types_list<D _ A>>>);
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
        DEFA(f11, v11, , sib::types_list<D _ A>);
        DEFA(f12, v12, , sib::types_list<D _ B>);
        DEFA(f13, v13, , sib::types_list<D _ A>);
        END;
    } {
        static constexpr int _C = 10;
        static constexpr int _I = 7;
        BEG;
        EXE(using Ts = gen_TL<_C>);
        MSG("    |", Types_to_Str<Ts>());
        MSG("    |type count = ", sib::types_info<Ts>::count);
        END;
        EXE(using H = sib::types_head_t<_I, Ts>);
        MSG("    |", Types_to_Str<H>());
        MSG("    |type count = ", sib::types_info<H>::count);
        END;

        EXE(using T = sib::types_tail_t<_I _ Ts>);
        MSG("    |", Types_to_Str<T>());
        MSG("    |type count = ", sib::types_info<T>::count);
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_tail_t<10, gen_TL<30>>);
        MSG("    |", Types_to_Str<Ts>());
        MSG("    |type count = ", sib::types_info<Ts>::count);
        END;

        EXE(using STs = sib::types_quick_sort_t<Ts>);
        MSG("    |", Types_to_Str<STs>());
        MSG("    |type count = ", sib::types_info<STs>::count);
        END;
    } {
        BEG;
        EXE(using STs = sib::types_quick_sort_t<sib::types_list<>>);
        MSG("    |", sib::static_type_name<STs>());
        MSG("    |type count = ", sib::types_info<STs>::count);
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_summ_t<gen_TL<5>, gen_TL<3>>);
        MSG("    |", Types_to_Str<Ts>());
        MSG("    |type count = ", sib::types_info<Ts>::count);
        END;

        EXE(using STs = sib::types_quick_sort_t<Ts>);
        MSG("    |", Types_to_Str<STs>());
        MSG("    |type count = ", sib::types_info<STs>::count);
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_list<sib::types_list<>, sib::types_list<>, int, sib::types_list<>, int, float, sib::types_list<>, int, sib::types_list<>>);
        MSG("    |", sib::static_type_name<Ts>());
        MSG("    |type count = ", sib::types_info<Ts>::count);
        END;

        EXE(using STs = sib::types_quick_sort_t<Ts>);
        MSG("    |", sib::static_type_name<STs>());
        MSG("    |type count = ", sib::types_info<STs>::count);
        END;
    }    

    sib::debug::outstream << std::endl;
    return 0;
}
