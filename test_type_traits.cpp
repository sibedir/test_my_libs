#include "test_type_traits.h"

#include <memory>
#include <type_traits>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

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

// TEST MACRO ----------------------------------------------------------------------------------------------------------

#define TM(ptr, def, arw, lptr, fn, lfn, arr, cont, ...)        \
    namespace SIB_CONCAT(test_type_traits_, __COUNTER__) {      \
        using T = __VA_ARGS__;                                  \
        static_assert(ptr  std::is_pointer_v        <T>);       \
        static_assert(def  sib::is_dereferenceable_v<T>);       \
        static_assert(arw  sib::has_arrow_v         <T>);       \
        static_assert(lptr sib::is_like_pointer_v   <T>);       \
        static_assert(                                          \
            not sib::LikePointer<T>                             \
            or (                                                \
                    sib::Pointer<T>                             \
                or  sib::LikeFunction<T>                        \
                or  sib::Same<deref_t<T>, contr_t<T>>           \
            )                                                   \
        );                                                      \
        static_assert(fn   sib::is_function_v       <T>);       \
        static_assert(lfn  sib::is_like_function_v  <T>);       \
        static_assert(arr  sib::is_array_v          <T>);       \
        static_assert(cont sib::is_container_v      <T>);       \
    }                                                           \

//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------------------------------------------|
//|  is ptr   |   deref   | has arrow | like ptr  |  is func  | like func |  is arr   | container |                    type                      |
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------------------------------------------|
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , void                                         )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , int                                          )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , ::sib::TWrapper<int>                         )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------------------------------------------|
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , ::std::nullptr_t                             )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    , ::sib::TNullPtr                              )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------------------------------------------|
TM(           ,    not    ,    not    ,           ,    not    ,    not    ,    not    ,    not    , void*                                        )
TM(           ,           ,    not    ,           ,    not    ,    not    ,    not    ,    not    , int*                                         )
TM(    not    ,    not    ,           ,           ,    not    ,    not    ,    not    ,    not    , ::sib::TPointer<void>                        )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::sib::TPointer<int>                         )
TM(    not    ,    not    ,           ,           ,    not    ,    not    ,    not    ,    not    , ::sib::TWrapper<void*>                       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::sib::TWrapper<int*>                        )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------------------------------------------|
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , TPointer2<int>                               )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::std::shared_ptr<int>                       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::std::unique_ptr<int>                       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::sib::TPointer<std::unique_ptr<int*>>       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::sib::TPointer<void*>                       )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::std::vector<int>::iterator                 )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::std::set<int>::iterator                    )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::std::vector<sib::TPointer<void>>::iterator )
TM(    not    ,           ,           ,           ,    not    ,    not    ,    not    ,    not    , ::std::vector<TPointer2<int>>::iterator      )
TM(           ,           ,    not    ,           ,    not    ,           ,    not    ,    not    , TFn*                                         )
TM(    not    ,           ,    not    ,           ,    not    ,           ,    not    ,    not    , ::sib::TPointer<TFn>                         )
TM(    not    ,           ,    not    ,           ,    not    ,           ,    not    ,    not    , ::sib::TWrapper<TFn*>                        )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------------------------------------------|
TM(    not    ,    not    ,    not    ,    not    ,           ,           ,    not    ,    not    , TFn                                          )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,           ,    not    ,    not    , ::std::function<TFn>                         )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,           ,    not    ,    not    , ::sib::TWrapper<TFn>                         )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------------------------------------------|
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           ,           , int[5]                                       )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           , ::sib::TArray<int, 5>                        )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           , ::std::array<int, 5>                         )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           , ::std::vector<int>                           )
TM(    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,    not    ,           , ::std::string                                )
//|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------------------------------------------|

#undef TM

// ---------------------------------------------------------------------------------------------------------------------

#define _ ,

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

    #define TM(lp, dt, at, ...)                                     \
    {                                                               \
        BEG;                                                        \
        MSG(#__VA_ARGS__);                                          \
        using T = __VA_ARGS__;                                      \
        if constexpr (std::is_default_constructible_v<T>)           \
            [[maybe_unused]] T tmp {};                              \
        ASS(lp sib::is_like_pointer_v<__VA_ARGS__>);                \
        TIS(deref_t<T>, dt);                                        \
        TIS(arrow_t<T>, at);                                        \
        END;                                                        \
    }

    {
        using namespace std;
        using namespace sib;

        //|-----------|-------------------|--------------------|---------------------------------------|
        //| like ptr  |     deref_t       |      arrow_t       |                 type                  |
        //|-----------|-------------------|--------------------|---------------------------------------|
        TM(    not    ,       None        ,        None        , int                                   );
        TM(    not    ,       None        ,        None        , TWrapper<int>                         );
        TM(    not    ,       None        ,        None        , nullptr_t                             );
        TM(    not    ,       None        ,        None        , TNullPtr                              );
        TM(           ,       None        ,        None        , void*                                 );
        TM(           ,       int&        ,        None        , int*                                  );
        TM(           ,       None        ,        void*       , TPointer<void>                        );
        TM(           ,       int&        ,        int*        , TPointer<int>                         );
        TM(           ,       None        ,        void*       , TWrapper<void*>                       );
        TM(           ,       int&        ,        int*        , TWrapper<int*>                        );
        TM(           ,       int&        ,    TPointer<int>   , TPointer2<int>                        );
        TM(           ,       int&        ,        int*        , shared_ptr<int>                       );
        TM(           ,       int&        ,        int*        , unique_ptr<int>                       );
        TM(           , unique_ptr<int*>& , unique_ptr<int*>*  , TPointer<unique_ptr<int*>>            );
        TM(           ,      void*&       ,       void**       , TPointer<void*>                       );
        TM(           ,       int&        ,        int*        , vector<int>::iterator                 );
        TM(           ,    int const &    ,     int const *    , set<int>::iterator                    );
        TM(           ,  TPointer<void>&  ,  TPointer<void>*   , vector<TPointer<void>>::iterator      );
        TM(           ,  TPointer2<int>&  ,  TPointer2<int>*   , vector<TPointer2<int>>::iterator      );
        TM(           , int (&&) (double) ,        None        , TFn*                                  );
        TM(           , int ( &) (double) ,        None        , TPointer<TFn>                         );
        TM(           , int ( &) (double) ,        None        , TWrapper<TFn*>                        );
        TM(    not    ,       None        ,        None        , function<TFn>                         );
        TM(    not    ,       None        ,        None        , TWrapper<TFn>                         );
        TM(    not    ,       None        ,        None        , int[5]                                );
        TM(    not    ,       None        ,        None        , TArray<int, 5>                        );
        TM(    not    ,       None        ,        None        , array<int, 5>                         );
        TM(    not    ,       None        ,        None        , vector<int>                           );
        TM(    not    ,       None        ,        None        , string                                );
        
        #undef TM
    }

    {
        BEG;
        ASS( sib::is_convertible_from_tooneof_v<int, float, std::string>);
        TIS( sib::convert_from_tooneof_select<int _ float _ std::string>, float);
        ASS(!sib::is_convertible_from_tooneof_v<int, float, char, std::string>);
        ASS(!sib::is_convertible_from_tooneof_v<int, std::string, std::vector<int>>);
        ASS( sib::is_convertible_from_tooneof_v<int, std::string, sib::TWrapper<float>, std::vector<int>>);
        TIS( sib::convert_from_tooneof_select<int _ std::string _ sib::TWrapper<float> _ std::vector<int>>, sib::TValue<float>);
        ASS(!sib::is_convertible_from_tooneof_v<int, std::string, sib::TWrapper<float>, std::vector<int>, sib::TWrapper<int>>);
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
        ASS(!std::is_convertible_v<C2, C4>);
        ASS( std::is_constructible_v<C4, C2>);
        ASS(!sib::is_convertible_from_to_v<C2, C4>);
        ASS( sib::is_constructible_to_from_v<C4, C2>);
        END;
        ASS(!sib::is_convertible_from_tooneof_v<C1, C2, C3>);
        ASS( sib::is_convertible_from_tooneof_v<C3, C1, C2, C1>);
        TIS(sib::convert_from_tooneof_select<C3 _ C1 _ C2 _ C1>, C2);
        ASS( sib::is_convertible_from_tooneof_v<C3, C2, C1, int>);
        TIS(sib::convert_from_tooneof_select<C3 _ C2 _ C1 _ int>, C2);
        ASS( sib::is_convertible_from_tooneof_v<C2, C2, C1, C3>);
        TIS(sib::convert_from_tooneof_select<C2 _ C2 _ C1 _ C3>, C2);
        ASS( sib::is_convertible_from_tooneof_v<C2, C1, int, C3>);
        TIS(sib::convert_from_tooneof_select<C2 _ C1 _ int _ C3>, int);
        END;
        ASS(!sib::is_convertible_to_fromoneof_v<C1, C2, C3>);
        ASS( sib::is_convertible_to_fromoneof_v<int, C1, C2, C3>);
        TIS(sib::convert_to_fromoneof_select<int _ C1 _ C2 _ C3>, C2);
        ASS( sib::is_convertible_to_fromoneof_v<C3, C2, C1, int>);
        ASS( sib::is_convertible_to_fromoneof_v<C3, C1, int, MyClass>);
        TIS(sib::convert_to_fromoneof_select<C3 _ C1 _ int _ MyClass>, int);
        ASS(!sib::is_convertible_to_fromoneof_v<C2, C2, C1, C3>);
        ASS( sib::is_convertible_to_fromoneof_v<C2, C2>);
        ASS(!sib::is_convertible_to_fromoneof_v<C2, C1>);
        ASS( sib::is_convertible_to_fromoneof_v<C2, C3>);
        ASS( sib::is_convertible_to_fromoneof_v<C2, C1, int, C3>);
        TIS(sib::convert_to_fromoneof_select<C2 _ C1 _ int _ C3>, C3);
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
        ASS(!sib::is_constructible_from_tooneof_v<C1, C2, C4, C3>);
        ASS( sib::is_constructible_from_tooneof_v<C1, C2, C1, C3>);
        TIS(sib::construct_from_tooneof_select<C1 _ C2 _ C1 _ C3>, C1);
        ASS( sib::is_constructible_from_tooneof_v<C4, C2, C1, C3>);
        TIS(sib::construct_from_tooneof_select<C4 _ C2 _ C1 _ C3>, C1);
        ASS(!sib::is_constructible_from_tooneof_v<C4, C2, C1, C4>);
        END;
        ASS(!sib::is_constructible_to_fromoneof_v<C5, C1, C2, C3>);
        ASS( sib::is_constructible_to_fromoneof_v<C1, C2, C4, C3>);
        TIS(sib::construct_to_fromoneof_select<C1 _ C2 _ C4 _ C3>, C4);
        ASS( sib::is_constructible_to_fromoneof_v<C1, C2, C1, C3>);
        TIS(sib::construct_to_fromoneof_select<C1 _ C2 _ C1 _ C3>, C1);
        ASS(!sib::is_constructible_to_fromoneof_v<C1, C2, C4, C1, C3>);
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

std::vector<std::string> gen_VS(size_t N)
{
    std::vector<std::string> res(N);
    for (int i = 0; i < N; ++i) res[i] = std::to_string(i);
    return res;
}

std::string VS_to_Str(std::vector<std::string> const & vec)
{
    std::string res = "<";
    if (vec.size() > 0)
    {
        for (auto& s : vec)
        {
            res += s;
            res += ", ";
        }
        res.resize(res.size() - 2);
    }
    res += '>';
    return res;
}

template <template <typename...> typename Tmpl>
struct gen_TS
{
private:
    template <size_t... idx_>
    static consteval auto impl(std::index_sequence<idx_...>)
    {
        return Tmpl< sib::int_tag<idx_> ... > {};
    }
public:
    template <size_t N>
    using type = decltype(impl(std::make_index_sequence<N>{}));
};

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

template <template <typename...> typename Tmpl, size_t N>
using gen_TS_t = typename gen_TS<Tmpl>::template type<N>;

template <size_t N> using gen_TP = decltype(gen_TP_impl(std::make_index_sequence<N>{}));
template <size_t N> using gen_TL = decltype(gen_TL_impl(std::make_index_sequence<N>{}));

template <typename T> struct TS_to_Str_Helper;

template <template <typename...> typename Tmpl, typename... Ts>
struct TS_to_Str_Helper<Tmpl<Ts...>>
{
    operator std::string() const
    {
        std::vector<std::string> vec;
        (vec.push_back(std::to_string(Ts::value)), ...);
        return VS_to_Str(vec);
    }
};

template <typename T>
std::string TS_to_Str() { return TS_to_Str_Helper<T>(); }

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

    #define _TS sib::types_pack
    #define _gen_TS(N) gen_TS_t<_TS, N>

    #define _C 10
    #define _I 7

    {
        BEG;
        TIS(A         , sib::types_first_t<A _ B _ C>);
        TIS(C         , sib::types_first_t<C _ B _ B>);
        TIS(D         , sib::types_first_t<D _ D _ B>);
      //TIS(          , sib::types_first_t<>);
        TIS(E         , sib::types_first_t<E>);
        TIS(A         , sib::types_first_t<_TS<A _ B _ C>>);
        TIS(C         , sib::types_first_t<_TS<C _ B _ B>>);
        TIS(D         , sib::types_first_t<_TS<D _ D _ B>>);
      //TIS(          , sib::types_first_t<_TS<>>);
        TIS(E         , sib::types_first_t<_TS<E>>);
        TIS(_TS<A _ E>, sib::types_first_t<_TS<A _ E> _ B _ C _ _TS<D _ A>>);
        TIS(_TS<D _ B>, sib::types_first_t<_TS<_TS<D _ B>>>);
        TIS(_TS<A _ E>, sib::types_first_t<_TS<_TS<A _ E> _ B _ C _ _TS<D _ A>>>);
        END;
    } {
        BEG;
        TIS(C                     , sib::types_last_t<A _ B _ C>);
        TIS(B                     , sib::types_last_t<C _ B _ B>);
        TIS(B                     , sib::types_last_t<D _ D _ B>);
      //TIS(                      , sib::types_last_t<>);
        TIS(E                     , sib::types_last_t<E>);
        TIS(C                     , sib::types_last_t<_TS<A _ B _ C>>);
        TIS(B                     , sib::types_last_t<_TS<C _ B _ B>>);
        TIS(B                     , sib::types_last_t<_TS<D _ D _ B>>);
      //TIS(                      , sib::types_last_t<_TS<>>);
        TIS(E                     , sib::types_last_t<_TS<E>>);
        TIS(_TS<D _ A>, sib::types_last_t<_TS<A _ E> _ B _ C _ _TS<D _ A>>);
        TIS(_TS<D _ B>, sib::types_last_t<_TS<_TS<D _ B>>>);
        TIS(_TS<D _ A>, sib::types_last_t<_TS<_TS<A _ E> _ B _ C _ _TS<D _ A>>>);
        END;
    } {
        BEG;
        EXE(using Ts = _gen_TS(_C));
        MSG("      ", TS_to_Str<Ts>());
        ASS(sib::types_info<Ts>::count == _C);
        END;

        EXE(using H = sib::types_head_t<_I, Ts>);
        MSG("      ", TS_to_Str<H>());
        ASS(sib::types_info<H>::count == _I);
        END;

        EXE(using T = sib::types_tail_t<_I _ Ts>);
        MSG("      ", TS_to_Str<T>());
        ASS(sib::types_info<T>::count == _I);
        END;
    } {
        BEG;
        EXE(using STs = sib::types_merge_sort_t<_TS<>>);
        TYP(STs);
        ASS(sib::types_info<STs>::count == 0);
        END;
    } {
        BEG;
        EXE(using STs = sib::types_quick_sort_t<_TS<>>);
        TYP(STs);
        ASS(sib::types_info<STs>::count == 0);
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_concat_t<_gen_TS(_C), _gen_TS(_I)>);
        MSG("      ", TS_to_Str<Ts>());
        ASS(sib::types_info<Ts>::count == _C + _I);
        END;

        EXE(using mSTs = sib::types_merge_sort_t<Ts>);
        MSG("      ", TS_to_Str<mSTs>());
        ASS(sib::types_info<mSTs>::count == _C + _I);
        END;

        using qSTs111 = sib::types_quick_sort_t<Ts>;

        EXE(using qSTs = sib::types_quick_sort_t<Ts>);
        MSG("      ", TS_to_Str<qSTs>());
        ASS(sib::types_info<qSTs>::count == _C + _I);
        END;

        ASS(TS_to_Str<mSTs>() == TS_to_Str<qSTs>());
        END;

        EXE(auto VS = gen_VS(_C));
        EXE(auto tmp = gen_VS(_I));
        EXE(VS.insert(VS.end(), tmp.begin(), tmp.end()));
        EXE(std::sort(VS.begin(), VS.end()));
        MSG("      ", VS_to_Str(VS));
        ASS(VS_to_Str(VS) == TS_to_Str<qSTs>());
        END;
    } {
        BEG;
        EXE(using Ts = _TS<_TS<>, _TS<>, int, _TS<>, A, float, _TS<>, int, _TS<>>);
        TYP(Ts);
        ASS(sib::types_info<Ts>::count == 9);
        END;

        EXE(using mSTs = sib::types_merge_sort_t<Ts>);
        TYP(mSTs);
        ASS(sib::types_info<mSTs>::count == 9);
        END;

        EXE(using qSTs = sib::types_quick_sort_t<Ts>);
        TYP(qSTs);
        ASS(sib::types_info<qSTs>::count == 9);
        END;

        EXE(using CmSTs = sib::types_erase_t<mSTs, _TS<>>);
        TYP(CmSTs);
        ASS(sib::types_info<CmSTs>::count == 4);
        END;

        EXE(using CqSTs = sib::types_erase_t<qSTs, _TS<>>);
        TYP(CqSTs);
        ASS(sib::types_info<CqSTs>::count == 4);
        END;

        ASS(sib::Same<CmSTs, CqSTs>);
        END;
    } {
        BEG;
        #undef _C
        #define _C 50
        EXE(using mSTs = sib::types_merge_sort_t<_gen_TS(_C)>);
        EXE(using qSTs = sib::types_quick_sort_t<_gen_TS(_C)>);
        ASS(sib::types_info<mSTs>::count == _C);
        ASS(sib::types_info<qSTs>::count == _C);
        END;
    }

    #undef _I
    #undef _C

    #undef _gen_TS
    #undef _TS

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
    
    #define _TS sib::types_list
    #define _gen_TS(N) gen_TS_t<_TS, N>

    #define _C 10
    #define _I 7

    {
        BEG;
        TIS(A         , sib::types_first_t<A _ B _ C>);
        TIS(C         , sib::types_first_t<C _ B _ B>);
        TIS(D         , sib::types_first_t<D _ D _ B>);
      //TIS(          , sib::types_first_t<>);
        TIS(E         , sib::types_first_t<E>);
        TIS(A         , sib::types_first_t<_TS<A _ B _ C>>);
        TIS(C         , sib::types_first_t<_TS<C _ B _ B>>);
        TIS(D         , sib::types_first_t<_TS<D _ D _ B>>);
      //TIS(          , sib::types_first_t<_TS<>>);
        TIS(E         , sib::types_first_t<_TS<E>>);
        TIS(_TS<A _ E>, sib::types_first_t<_TS<A _ E> _ B _ C _ _TS<D _ A>>);
        TIS(_TS<D _ B>, sib::types_first_t<_TS<_TS<D _ B>>>);
        TIS(_TS<A _ E>, sib::types_first_t<_TS<_TS<A _ E> _ B _ C _ _TS<D _ A>>>);
        END;
    } {
        BEG;
        TIS(C                     , sib::types_last_t<A _ B _ C>);
        TIS(B                     , sib::types_last_t<C _ B _ B>);
        TIS(B                     , sib::types_last_t<D _ D _ B>);
      //TIS(                      , sib::types_last_t<>);
        TIS(E                     , sib::types_last_t<E>);
        TIS(C                     , sib::types_last_t<_TS<A _ B _ C>>);
        TIS(B                     , sib::types_last_t<_TS<C _ B _ B>>);
        TIS(B                     , sib::types_last_t<_TS<D _ D _ B>>);
      //TIS(                      , sib::types_last_t<_TS<>>);
        TIS(E                     , sib::types_last_t<_TS<E>>);
        TIS(_TS<D _ A>, sib::types_last_t<_TS<A _ E> _ B _ C _ _TS<D _ A>>);
        TIS(_TS<D _ B>, sib::types_last_t<_TS<_TS<D _ B>>>);
        TIS(_TS<D _ A>, sib::types_last_t<_TS<_TS<A _ E> _ B _ C _ _TS<D _ A>>>);
        END;
    } {
        BEG;
        EXE(using Ts = _gen_TS(_C));
        MSG("      ", TS_to_Str<Ts>());
        ASS(sib::types_info<Ts>::count == _C);
        END;

        EXE(using H = sib::types_head_t<_I, Ts>);
        MSG("      ", TS_to_Str<H>());
        ASS(sib::types_info<H>::count == _I);
        END;

        EXE(using T = sib::types_tail_t<_I _ Ts>);
        MSG("      ", TS_to_Str<T>());
        ASS(sib::types_info<T>::count == _I);
        END;
    } {
        BEG;
        EXE(using STs = sib::types_merge_sort_t<_TS<>>);
        TYP(STs);
        ASS(sib::types_info<STs>::count == 0);
        END;
    } {
        BEG;
        EXE(using STs = sib::types_quick_sort_t<_TS<>>);
        TYP(STs);
        ASS(sib::types_info<STs>::count == 0);
        END;
    } {
        BEG;
        EXE(using Ts = sib::types_concat_t<_gen_TS(_C), _gen_TS(_I)>);
        MSG("      ", TS_to_Str<Ts>());
        ASS(sib::types_info<Ts>::count == _C + _I);
        END;

        EXE(using mSTs = sib::types_merge_sort_t<Ts>);
        MSG("      ", TS_to_Str<mSTs>());
        ASS(sib::types_info<mSTs>::count == _C + _I);
        END;

        EXE(using qSTs = sib::types_quick_sort_t<Ts>);
        MSG("      ", TS_to_Str<qSTs>());
        ASS(sib::types_info<qSTs>::count == _C + _I);
        END;

        ASS(TS_to_Str<mSTs>() == TS_to_Str<qSTs>());
        END;

        EXE(auto VS = gen_VS(_C));
        EXE(auto tmp = gen_VS(_I));
        EXE(VS.insert(VS.end(), tmp.begin(), tmp.end()));
        EXE(std::sort(VS.begin(), VS.end()));
        MSG("      ", VS_to_Str(VS));
        ASS(VS_to_Str(VS) == TS_to_Str<qSTs>());
        END;
    } {
        BEG;
        EXE(using Ts = _TS<_TS<>, _TS<>, int, _TS<>, A, float, _TS<>, int, _TS<>>);
        TYP(Ts);
        ASS(sib::types_info<Ts>::count == 9);
        END;

        EXE(using mSTs = sib::types_merge_sort_t<Ts>);
        TYP(mSTs);
        ASS(sib::types_info<mSTs>::count == 9);
        END;

        EXE(using qSTs = sib::types_quick_sort_t<Ts>);
        TYP(qSTs);
        ASS(sib::types_info<qSTs>::count == 9);
        END;

        EXE(using CmSTs = sib::types_erase_t<mSTs, _TS<>>);
        TYP(CmSTs);
        ASS(sib::types_info<CmSTs>::count == 4);
        END;

        EXE(using CqSTs = sib::types_erase_t<qSTs, _TS<>>);
        TYP(CqSTs);
        ASS(sib::types_info<CqSTs>::count == 4);
        END;

        ASS(sib::Same<CmSTs, CqSTs>);
        END;
    } {
        BEG;
        #undef _C
        #define _C 50
        EXE(using mSTs = sib::types_merge_sort_t<_gen_TS(_C)>);
        EXE(using qSTs = sib::types_quick_sort_t<_gen_TS(_C)>);
        ASS(sib::types_info<mSTs>::count == _C);
        ASS(sib::types_info<qSTs>::count == _C);
        END;
    }

    #undef _I
    #undef _C

    #undef _gen_TS
    #undef _TS

    sib::debug::outstream << std::endl;
    return 0;
}
