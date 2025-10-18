#pragma once

#include <type_traits>
#include <string_view>
#include <functional>
#include <iterator>

#include "sib_support.h"

namespace sib {

    using ::std::size_t;

    // ----------------------------------------------------------------------------------- ADVANCE DECLARATIONS

    struct types_container  // base type for containers of types
    {
        static constexpr size_t count();
    };

    // ----------------------------------------------------------------------------------- static type name

    /* https://stackoverflow.com/a/64490578/23601704 */

    namespace detail {

        template <typename T>
        constexpr ::std::string_view SIB_STN() noexcept
        {
            #if defined(__clang__)
                return __PRETTY_FUNCTION__;
            #elif defined(__GNUC__)
                return __PRETTY_FUNCTION__;
            #elif defined(_MSC_VER)
                return __FUNCSIG__;
            #else
                #error "Unsupported compiler!"
            #endif
        }

    }
    
    template <typename T>
    constexpr ::std::string_view static_type_name() noexcept
    {
        constexpr auto prefix = detail::SIB_STN<void>().find("void");
        constexpr auto suffix = detail::SIB_STN<void>().size() - prefix - 4;

        auto name = detail::SIB_STN<T>();

        name.remove_prefix(prefix);
        name.remove_suffix(suffix);

        return name;
    }


    template <typename T>
    constexpr auto static_type_name(T&& arg) noexcept
    {
        return static_type_name<decltype(std::forward<T>(arg))>();
    }



    // ----------------------------------------------------------------------------------- tags

    template<int N>
    struct int_tag
    {
        static constexpr int value = N;
    };

    template<typename T>
    struct type_tag
    {
        using type = T;
    };
    
    
    
    // ----------------------------------------------------------------------------------- peculiarities
    
    inline constexpr bool NON_STANDARD_FUNC_PTR_CONVERSION_DETECT = ::std::is_convertible_v<void(*)(), void const *>;
    
    #ifdef _MSC_VER
    #ifndef DISABLE_SIB_WARNINGS
    #ifndef DISABLE_WARNING_NON_STANDARD_FUNC_PTR_CONVERSION
    
        #pragma message (__FILE__ "(" SIB_STR_STRINGISE(__LINE__) ") : WARNING: "               \
            "Warning [SIB]: Possible non-standard conversion of function pointer to void*. "    \
            "Check the ::sib::NON_STANDARD_FUNC_PTR_CONVERSION_DETECT variable. "               \
            "To suppress this warning, define the "                                             \
                                   "DISABLE_WARNING_NON_STANDARD_FUNC_PTR_CONVERSION macros."   \
        )                                                                                       \
        
    #endif
    #endif
    #endif
    


    // ----------------------------------------------------------------------------------- true for some/all/one

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
        requires (sizeof...(Ts) > 0)
    inline constexpr bool true_for_some_v = (static_cast<bool>(BinaryPredicateTmpl<T, Ts>()) or ...);

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    using true_for_some = ::std::bool_constant<true_for_some_v<T, BinaryPredicateTmpl, Ts...>>;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    concept TrueForSome = true_for_some_v<T, BinaryPredicateTmpl, Ts...>;



    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
        requires (sizeof...(Ts) > 0)
    inline constexpr bool true_for_all_v = (static_cast<bool>(BinaryPredicateTmpl<T, Ts>()) and ...);

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    using true_for_all = ::std::bool_constant<true_for_all_v<T, BinaryPredicateTmpl, Ts...>>;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    concept TrueForAll = true_for_all_v<T, BinaryPredicateTmpl, Ts...>;



    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
        requires (sizeof...(Ts) > 0)
    inline constexpr bool true_for_one_v = (static_cast<bool>(BinaryPredicateTmpl<T, Ts>()) + ...) == 1;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    using true_for_one = ::std::bool_constant<true_for_one_v<T, BinaryPredicateTmpl, Ts...>>;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    concept TrueForOne = true_for_one_v<T, BinaryPredicateTmpl, Ts...>;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    struct true_for_one_select;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    using true_for_one_select_t = typename true_for_one_select<T, BinaryPredicateTmpl, Ts...>::type;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename F, typename... Ts>
        requires ( true_for_one_v<T, BinaryPredicateTmpl, F> )
    struct true_for_one_select<T, BinaryPredicateTmpl, F, Ts...>
    {
        using type = F;
    };

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename F, typename... Ts>
        requires ( not true_for_one_v<T, BinaryPredicateTmpl, F    >
                   and true_for_one_v<T, BinaryPredicateTmpl, Ts...> )
    struct true_for_one_select<T, BinaryPredicateTmpl, F, Ts...>
    {
        using type = true_for_one_select_t<T, BinaryPredicateTmpl, Ts...>;
    };



    // ----------------------------------------------------------------------------------- false for some/all/one

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
        requires (sizeof...(Ts) > 0)
    inline constexpr bool false_for_some_v = not (static_cast<bool>(BinaryPredicateTmpl<T, Ts>()) and ...);

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    using false_for_some = ::std::bool_constant<false_for_some_v<T, BinaryPredicateTmpl, Ts...>>;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    concept FalseForSome = false_for_some_v<T, BinaryPredicateTmpl, Ts...>;



    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
        requires (sizeof...(Ts) > 0)
    inline constexpr bool false_for_all_v = not (static_cast<bool>(BinaryPredicateTmpl<T, Ts>()) or ...);

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    using false_for_all = ::std::bool_constant<false_for_all_v<T, BinaryPredicateTmpl, Ts...>>;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    concept FalseForAll = false_for_all_v<T, BinaryPredicateTmpl, Ts...>;



    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
        requires (sizeof...(Ts) > 0)
    inline constexpr bool false_for_one_v = (static_cast<bool>(BinaryPredicateTmpl<T, Ts>()) + ... ) == (sizeof...(Ts) - 1);

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    using false_for_one = ::std::bool_constant<false_for_one_v<T, BinaryPredicateTmpl, Ts...>>;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    concept FalseForOne = false_for_one_v<T, BinaryPredicateTmpl, Ts...>;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    struct false_for_one_select;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename... Ts>
    using false_for_one_select_t = typename false_for_one_select<T, BinaryPredicateTmpl, Ts...>::type;

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename F, typename... Ts>
        requires ( false_for_one_v<T, BinaryPredicateTmpl, F> )
    struct false_for_one_select<T, BinaryPredicateTmpl, F, Ts...>
    {
        using type = F;
    };

    template <typename T, template <typename, typename> typename BinaryPredicateTmpl, typename F, typename... Ts>
        requires ( not false_for_one_v<T, BinaryPredicateTmpl, F    >
                   and false_for_one_v<T, BinaryPredicateTmpl, Ts...> )
    struct false_for_one_select<T, BinaryPredicateTmpl, F, Ts...>
    {
        using type = false_for_one_select_t<T, BinaryPredicateTmpl, Ts...>;
    };



    static_assert(TrueForOne<int, ::std::is_same, char, int, double>);
    static_assert(std::is_same_v<
        true_for_one_select_t<int, ::std::is_same, char, int, double>,
        int
    >);

    static_assert(TrueForOne<double, ::std::is_same, int, double, float>);
    static_assert(std::is_same_v<
        true_for_one_select_t<double, ::std::is_same, int, double, float>,
        double
    >);


    // ----------------------------------------------------------------------------------- compare types

    template <typename L, typename R>
    inline constexpr bool type_equal_v = ::std::is_same_v<L, R>;

    template <typename L, typename R>
    inline constexpr bool type_less_v = (sib::static_type_name<L>() < ::sib::static_type_name<R>());

    template <typename L, typename R>
    inline constexpr bool type_more_v = (sib::static_type_name<L>() > ::sib::static_type_name<R>());



    // ----------------------------------------------------------------------------------- template instances

    template <typename, template <typename...> typename>
    inline constexpr bool is_instantiation_of_v = false;

    template <template <typename...> typename Tmpl, typename... Ts>
    inline constexpr bool is_instantiation_of_v<Tmpl<Ts...>, Tmpl> = true;

    template <typename T, template <typename...> typename Tmpl>
    concept    InstantiationOf =     is_instantiation_of_v<T, Tmpl>;
    
    template <typename T, template <typename...> typename Tmpl>
    concept NotInstantiationOf = not is_instantiation_of_v<T, Tmpl>;

    
    template <template <typename...> typename Tmpl, typename... Ts>
    struct instantiate_templ
    {
        using type = Tmpl<Ts...>;
    };

    template <template <typename...> typename Tmpl, template <typename...> typename TsTmpl, typename... Ts>
        requires(std::is_base_of_v<types_container, TsTmpl<>>)
    struct instantiate_templ<Tmpl, TsTmpl<Ts...>>
    {
        using type = Tmpl<Ts...>;
    };

    template <template <typename...> typename Tmpl, typename... Ts>
    using instantiate_templ_t = typename instantiate_templ<Tmpl, Ts...>::type;
    
    
    
    template <template <typename...> typename Tmpl, typename Like>
    struct instantiate_like
    {
        static_assert(always_false_v<Like>, "Template parameter Like is not instantiate of template.");
    };
    
    template <template <typename...> typename Tmpl, template <typename...> typename Like, typename... Ts>
    struct instantiate_like<Tmpl, Like<Ts...>>
    {
        using type = Tmpl<Ts...>;
    };
    
    template <template <typename...> typename Tmpl, typename Like>
    using instantiate_like_t = typename instantiate_like<Tmpl, Like>::type;



    template <typename Left, typename Right>
    struct swap_template_param;
    
    template <template <typename...> typename LTmpl, template <typename...> typename RTmpl, typename... LTs, typename... RTs>
    struct swap_template_param<LTmpl<LTs...>, RTmpl<RTs...>>
    {
        using left  = LTmpl<RTs...>;
        using right = RTmpl<LTs...>;
    };
    
    template <typename TmplBasis, typename Example>
    using instantiate_from_example = typename swap_template_param<TmplBasis, Example>::left;
    
    

    // ----------------------------------------------------------------------------------- conditional_v

    template <bool Test, auto Val1, auto Val2 = decltype(Val1){}>
    inline constexpr auto conditional_v = Val1;

    template <auto Val1, auto Val2>
    inline constexpr auto conditional_v<false, Val1, Val2> = Val2;

    // ----------------------------------------------------------------------------------- is same

    template <typename T, typename... Ts>
    inline constexpr bool is_any_of_v = true_for_some_v<T, ::std::is_same, Ts...>;

    template <typename T, typename... Ts>
    inline constexpr bool is_one_of_v = true_for_one_v <T, ::std::is_same, Ts...>;

    template <typename T, typename    U > concept    Same  =     ::std::is_same_v  <T, U    >;
    template <typename T, typename... Ts> concept    AnyOf =            is_any_of_v<T, Ts...>;
    template <typename T, typename... Ts> concept    OneOf =            is_one_of_v<T, Ts...>;

    template <typename T, typename    U > concept NotSame  = not ::std::is_same_v  <T, U    >;
    template <typename T, typename... Ts> concept NotAnyOf = not        is_any_of_v<T, Ts...>;
    template <typename T, typename... Ts> concept NotOneOf = not        is_one_of_v<T, Ts...>;
    


    // ----------------------------------------------------------------------------------- constract/convert/cast

    template <typename From, typename To> inline constexpr bool is_constructible_from_to_v = ::std::is_constructible_v<To, From>;
    template <typename From, typename To> inline constexpr bool   is_convertible_from_to_v = ::std::is_convertible_v<From, To>;
    template <typename From, typename To> inline constexpr bool      is_castable_from_to_v = requires { static_cast<To>(std::declval<From>()); };
    template <typename To, typename From> inline constexpr bool is_constructible_to_from_v = ::std::is_constructible_v<To, From>;
    template <typename To, typename From> inline constexpr bool   is_convertible_to_from_v = ::std::is_convertible_v<From, To>;
    template <typename To, typename From> inline constexpr bool      is_castable_to_from_v = requires { static_cast<To>(std::declval<From>()); };

    template <typename From, typename To> using is_constructible_from_to = ::std::bool_constant<is_constructible_from_to_v<From, To>>;
    template <typename From, typename To> using   is_convertible_from_to = ::std::bool_constant<  is_convertible_from_to_v<From, To>>;
    template <typename From, typename To> using      is_castable_from_to = ::std::bool_constant<     is_castable_from_to_v<From, To>>;
    template <typename To, typename From> using is_constructible_to_from = ::std::bool_constant<is_constructible_to_from_v<To, From>>;
    template <typename To, typename From> using   is_convertible_to_from = ::std::bool_constant<  is_convertible_to_from_v<To, From>>;
    template <typename To, typename From> using      is_castable_to_from = ::std::bool_constant<     is_castable_to_from_v<To, From>>;

    template <typename From, typename... To> inline constexpr bool is_constructible_from_toanyof_v = true_for_some_v<From, is_constructible_from_to, To  ...>;
    template <typename From, typename... To> inline constexpr bool   is_convertible_from_toanyof_v = true_for_some_v<From,   is_convertible_from_to, To  ...>;
    template <typename From, typename... To> inline constexpr bool      is_castable_from_toanyof_v = true_for_some_v<From,      is_castable_from_to, To  ...>;
    template <typename To, typename... From> inline constexpr bool is_constructible_to_fromanyof_v = true_for_some_v<To  , is_constructible_to_from, From...>;
    template <typename To, typename... From> inline constexpr bool   is_convertible_to_fromanyof_v = true_for_some_v<To  ,   is_convertible_to_from, From...>;
    template <typename To, typename... From> inline constexpr bool      is_castable_to_fromanyof_v = true_for_some_v<To  ,      is_castable_to_from, From...>;

    template <typename From, typename... To> inline constexpr bool is_constructible_from_tooneof_v = true_for_one_v <From, is_constructible_from_to, To  ...>;
    template <typename From, typename... To> inline constexpr bool   is_convertible_from_tooneof_v = true_for_one_v <From,   is_convertible_from_to, To  ...>;
    template <typename From, typename... To> inline constexpr bool      is_castable_from_tooneof_v = true_for_one_v <From,      is_castable_from_to, To  ...>;
    template <typename To, typename... From> inline constexpr bool is_constructible_to_fromoneof_v = true_for_one_v <To  , is_constructible_to_from, From...>;
    template <typename To, typename... From> inline constexpr bool   is_convertible_to_fromoneof_v = true_for_one_v <To  ,   is_convertible_to_from, From...>;
    template <typename To, typename... From> inline constexpr bool      is_castable_to_fromoneof_v = true_for_one_v <To  ,      is_castable_to_from, From...>;

    template <typename From, typename    To> concept    ConstructibleTo        =     is_constructible_from_to_v     <From, To   >;
    template <typename From, typename    To> concept      ConvertibleTo        =       is_convertible_from_to_v     <From, To   >;
    template <typename From, typename    To> concept         CastableTo        =          is_castable_from_to_v     <From, To   >;
    template <typename From, typename... To> concept    ConstructibleToAnyOf   =     is_constructible_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept      ConvertibleToAnyOf   =       is_convertible_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept         CastableToAnyOf   =          is_castable_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept    ConstructibleToOneOf   =     is_constructible_from_tooneof_v<From, To...>;
    template <typename From, typename... To> concept      ConvertibleToOneOf   =       is_convertible_from_tooneof_v<From, To...>;
    template <typename From, typename... To> concept         CastableToOneOf   =          is_castable_from_tooneof_v<From, To...>;
    template <typename To, typename    From> concept    ConstructibleFrom      =     is_constructible_to_from_v     <To, From   >;
    template <typename To, typename    From> concept      ConvertibleFrom      =       is_convertible_to_from_v     <To, From   >;
    template <typename To, typename    From> concept         CastableFrom      =          is_castable_to_from_v     <To, From   >;
    template <typename To, typename... From> concept    ConstructibleFromAnyOf =     is_constructible_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept      ConvertibleFromAnyOf =       is_convertible_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept         CastableFromAnyOf =          is_castable_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept    ConstructibleFromOneOf =     is_constructible_to_fromoneof_v<To, From...>;
    template <typename To, typename... From> concept      ConvertibleFromOneOf =       is_convertible_to_fromoneof_v<To, From...>;
    template <typename To, typename... From> concept         CastableFromOneOf =          is_castable_to_fromoneof_v<To, From...>;

    template <typename From, typename    To> concept NotConstructibleTo        = not is_constructible_from_to_v     <From, To   >;
    template <typename From, typename    To> concept   NotConvertibleTo        = not   is_convertible_from_to_v     <From, To   >;
    template <typename From, typename    To> concept      NotCastableTo        = not      is_castable_from_to_v     <From, To   >;
    template <typename From, typename... To> concept NotConstructibleToAnyOf   = not is_constructible_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept   NotConvertibleToAnyOf   = not   is_convertible_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept      NotCastableToAnyOf   = not      is_castable_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept NotConstructibleToOneOf   = not is_constructible_from_tooneof_v<From, To...>;
    template <typename From, typename... To> concept   NotConvertibleToOneOf   = not   is_convertible_from_tooneof_v<From, To...>;
    template <typename From, typename... To> concept      NotCastableToOneOf   = not      is_castable_from_tooneof_v<From, To...>;
    template <typename To, typename    From> concept NotConstructibleFrom      = not is_constructible_to_from_v     <To, From   >;
    template <typename To, typename    From> concept   NotConvertibleFrom      = not   is_convertible_to_from_v     <To, From   >;
    template <typename To, typename    From> concept      NotCastableFrom      = not      is_castable_to_from_v     <To, From   >;
    template <typename To, typename... From> concept NotConstructibleFromAnyOf = not is_constructible_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept   NotConvertibleFromAnyOf = not   is_convertible_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept      NotCastableFromAnyOf = not      is_castable_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept NotConstructibleFromOneOf = not is_constructible_to_fromoneof_v<To, From...>;
    template <typename To, typename... From> concept   NotConvertibleFromOneOf = not   is_convertible_to_fromoneof_v<To, From...>;
    template <typename To, typename... From> concept      NotCastableFromOneOf = not      is_castable_to_fromoneof_v<To, From...>;



    template <typename From, typename... To> requires(is_constructible_from_tooneof_v<From, To...>) using construct_from_tooneof_select = true_for_one_select_t<From, is_constructible_from_to, To  ...>;
    template <typename From, typename... To> requires(  is_convertible_from_tooneof_v<From, To...>) using   convert_from_tooneof_select = true_for_one_select_t<From,   is_convertible_from_to, To  ...>;
    template <typename From, typename... To> requires(     is_castable_from_tooneof_v<From, To...>) using      cast_from_tooneof_select = true_for_one_select_t<From,      is_castable_from_to, To  ...>;
    template <typename To, typename... From> requires(is_constructible_to_fromoneof_v<To, From...>) using construct_to_fromoneof_select = true_for_one_select_t<To  , is_constructible_to_from, From...>;
    template <typename To, typename... From> requires(  is_convertible_to_fromoneof_v<To, From...>) using   convert_to_fromoneof_select = true_for_one_select_t<To  ,   is_convertible_to_from, From...>;
    template <typename To, typename... From> requires(     is_castable_to_fromoneof_v<To, From...>) using      cast_to_fromoneof_select = true_for_one_select_t<To  ,      is_castable_to_from, From...>;



    // ----------------------------------------------------------------------------------- assign

    template <typename Source, typename    Dest> inline constexpr bool is_assignable_from_to_v = ::std::is_assignable_v<Dest, Source>;
    template <typename Dest, typename    Source> inline constexpr bool is_assignable_to_from_v = ::std::is_assignable_v<Dest, Source>;

    template <typename Source, typename    Dest> using is_assignable_from_to = ::std::bool_constant<::std::is_assignable_v<Dest, Source>>;
    template <typename Dest, typename    Source> using is_assignable_to_from = ::std::bool_constant<::std::is_assignable_v<Dest, Source>>;

    template <typename Source, typename... Dest> inline constexpr bool is_assignable_from_toanyof_v = true_for_some_v<Source, is_assignable_from_to, Dest...>;
    template <typename Dest, typename... Source> inline constexpr bool is_assignable_to_fromanyof_v = true_for_some_v<Dest, is_assignable_to_from, Source...>;

    template <typename Source, typename... Dest> inline constexpr bool is_assignable_from_tooneof_v = true_for_one_v<Source, is_assignable_from_to, Dest...>;
    template <typename Dest, typename... Source> inline constexpr bool is_assignable_to_fromoneof_v = true_for_one_v<Dest, is_assignable_to_from, Source...>;

    template <typename Source, typename    Dest> concept    AssignableFromTo      =     is_assignable_from_to_v     <Source, Dest   >;
    template <typename Source, typename... Dest> concept    AssignableFromToAnyOf =     is_assignable_from_toanyof_v<Source, Dest...>;
    template <typename Source, typename... Dest> concept    AssignableFromToOneOf =     is_assignable_from_tooneof_v<Source, Dest...>;
    template <typename Dest, typename    Source> concept    AssignableToFrom      =     is_assignable_to_from_v     <Dest, Source   >;
    template <typename Dest, typename... Source> concept    AssignableToFromAnyOf =     is_assignable_to_fromanyof_v<Dest, Source...>;
    template <typename Dest, typename... Source> concept    AssignableToFromOneOf =     is_assignable_to_fromoneof_v<Dest, Source...>;

    template <typename Source, typename    Dest> concept NotAssignableTo        = not is_assignable_from_to_v     <Source, Dest   >;
    template <typename Source, typename... Dest> concept NotAssignableToAnyOf   = not is_assignable_from_toanyof_v<Source, Dest...>;
    template <typename Source, typename... Dest> concept NotAssignableToOneOf   = not is_assignable_from_tooneof_v<Source, Dest...>;
    template <typename Dest, typename    Source> concept NotAssignableFrom      = not is_assignable_to_from_v     <Dest, Source   >;
    template <typename Dest, typename... Source> concept NotAssignableFromAnyOf = not is_assignable_to_fromanyof_v<Dest, Source...>;
    template <typename Dest, typename... Source> concept NotAssignableFromOneOf = not is_assignable_to_fromoneof_v<Dest, Source...>;


    template <typename Source, typename... Dest>
        requires( is_assignable_from_tooneof_v<Source, Dest...> )
    struct assign_from_tooneof
    {
        using select = true_for_one_select_t<Source, is_assignable_from_to, Dest...>;
        using result = decltype(std::declval<select>() = ::std::declval<Source>());
    };
    
    template <typename Dest, typename... Source>
        requires( is_assignable_to_fromoneof_v<Dest, Source...> )
    struct assign_to_fromoneof
    {
        using select = true_for_one_select_t<Dest, is_assignable_to_from, Source...>;
        using result = decltype(std::declval<Dest>() = ::std::declval<select>());
    };

    template <typename Source, typename... Dest> requires(is_assignable_from_tooneof_v<Source, Dest...>) using assign_from_tooneof_select = typename assign_from_tooneof<Source, Dest...>::select;
    template <typename Source, typename... Dest> requires(is_assignable_from_tooneof_v<Source, Dest...>) using assign_from_tooneof_result = typename assign_from_tooneof<Source, Dest...>::result;
    template <typename Dest, typename... Source> requires(is_assignable_to_fromoneof_v<Dest, Source...>) using assign_to_fromoneof_select = typename assign_to_fromoneof<Dest, Source...>::select;
    template <typename Dest, typename... Source> requires(is_assignable_to_fromoneof_v<Dest, Source...>) using assign_to_fromoneof_result = typename assign_to_fromoneof<Dest, Source...>::result;



    // ----------------------------------------------------------------------------------- compare

    template <typename L, typename R> inline constexpr bool has_equal_v         = requires (L l, R r) { l == r; };
    template <typename L, typename R> inline constexpr bool has_not_equal_v     = requires (L l, R r) { l != r; };
    template <typename L, typename R> inline constexpr bool has_less_v          = requires (L l, R r) { l <  r; };
    template <typename L, typename R> inline constexpr bool has_greater_v       = requires (L l, R r) { l >  r; };
    template <typename L, typename R> inline constexpr bool has_less_equal_v    = requires (L l, R r) { l <= r; };
    template <typename L, typename R> inline constexpr bool has_greater_equal_v = requires (L l, R r) { l >= r; };

    template <typename L, typename R> using has_equal         = ::std::bool_constant<has_equal_v        <L, R>>;
    template <typename L, typename R> using has_not_equal     = ::std::bool_constant<has_not_equal_v    <L, R>>;
    template <typename L, typename R> using has_less          = ::std::bool_constant<has_less_v         <L, R>>;
    template <typename L, typename R> using has_greater       = ::std::bool_constant<has_greater_v      <L, R>>;
    template <typename L, typename R> using has_less_equal    = ::std::bool_constant<has_less_equal_v   <L, R>>;
    template <typename L, typename R> using has_greater_equal = ::std::bool_constant<has_greater_equal_v<L, R>>;

    template <typename L, typename R> concept HasEqual        = has_equal_v        <L, R>;
    template <typename L, typename R> concept HasNotEqual     = has_not_equal_v    <L, R>;
    template <typename L, typename R> concept HasLess         = has_less_v         <L, R>;
    template <typename L, typename R> concept HasGreater      = has_greater_v      <L, R>;
    template <typename L, typename R> concept HasLessEqual    = has_less_equal_v   <L, R>;
    template <typename L, typename R> concept HasGreaterEqual = has_greater_equal_v<L, R>;

    template <typename L, typename R> requires (has_equal_v        <L, R>) struct equal_result         { using type = decltype(std::declval<L> == ::std::declval<R>); };
    template <typename L, typename R> requires (has_not_equal_v    <L, R>) struct not_equal_result     { using type = decltype(std::declval<L> != ::std::declval<R>); };
    template <typename L, typename R> requires (has_less_v         <L, R>) struct less_result          { using type = decltype(std::declval<L> <  ::std::declval<R>); };
    template <typename L, typename R> requires (has_greater_v      <L, R>) struct greater_result       { using type = decltype(std::declval<L> >  ::std::declval<R>); };
    template <typename L, typename R> requires (has_less_equal_v   <L, R>) struct less_equal_result    { using type = decltype(std::declval<L> <= ::std::declval<R>); };
    template <typename L, typename R> requires (has_greater_equal_v<L, R>) struct greater_equal_result { using type = decltype(std::declval<L> >= ::std::declval<R>); };

    template <typename L, typename R> using equal_result_t         = typename equal_result        <L, R>::type;
    template <typename L, typename R> using not_equal_result_t     = typename not_equal_result    <L, R>::type;
    template <typename L, typename R> using less_result_t          = typename less_result         <L, R>::type;
    template <typename L, typename R> using greater_result_t       = typename greater_result      <L, R>::type;
    template <typename L, typename R> using less_equal_result_t    = typename less_equal_result   <L, R>::type;
    template <typename L, typename R> using greater_equal_result_t = typename greater_equal_result<L, R>::type;

    // ----------------------------------------------------------------------------------- container

    template <typename T>
    inline constexpr bool is_container_v = // requires (T v) { for (auto it : v) {} } // до лучших времён
        requires(T & v)
        {
            { ::std::begin(v) == ::std::end(v) } -> ConvertibleTo<bool>;
            requires requires(decltype(std::begin(v)) it)
            {
                { *it };
                { ++it } -> ::std::same_as<decltype(it)&>;
            };
        }
    ;

    template <typename T> concept    Container =     is_container_v<T>;
    template <typename T> concept NotContainer = not is_container_v<T>;

    template <Container Cont>
    using container_elem_t = decltype(*std::begin(std::declval<Cont&>()));



    // ----------------------------------------------------------------------------------- class

    template <typename T> inline constexpr bool is_class_v = ::std::is_class_v<T>;

    template <typename T> concept    Class =     is_class_v<T>;
    template <typename T> concept NotClass = not is_class_v<T>;



    // ----------------------------------------------------------------------------------- arithmetic

    template <typename T> inline constexpr bool is_arithmetic_v = ::std::is_arithmetic_v<T>;

    template <typename T> concept    Arithmetic =     is_arithmetic_v<T>;
    template <typename T> concept NotArithmetic = not is_arithmetic_v<T>;



    // ----------------------------------------------------------------------------------- enum

    template <typename T> inline constexpr bool is_enum_v = ::std::is_enum_v<T>;

    template <typename T> concept    Enum =     is_enum_v<T>;
    template <typename T> concept NotEnum = not is_enum_v<T>;



    template <typename T>
    struct is_enum_class : ::std::false_type {};

    template <Enum E> requires (not ::std::is_convertible_v<E, ::std::underlying_type_t<::std::remove_cvref_t<E>>>)
    struct is_enum_class<E> : ::std::true_type {};

    template <typename T> inline constexpr bool  is_enum_class_v = is_enum_class<T>::value;

    template <typename T> concept    EnumClass =     is_enum_class_v<T>;
    template <typename T> concept NotEnumClass = not is_enum_class_v<T>;



    // ----------------------------------------------------------------------------------- function

    template <typename T> inline constexpr bool is_function_v = ::std::is_function_v<T>;

    template <typename T> concept    Function =     is_function_v<T>;
    template <typename T> concept NotFunction = not is_function_v<T>;



    template <typename T>
    inline constexpr bool is_like_function_v = requires (T f) { ::std::function(f); };
    
    template <typename T> concept    LikeFunction =     is_like_function_v<T>;
    template <typename T> concept NotLikeFunction = not is_like_function_v<T>;



    // ----------------------------------------------------------------------------------- pointer

    template <typename T> concept    Pointer =     ::std::is_pointer_v<T>;
    template <typename T> concept NotPointer = not ::std::is_pointer_v<T>;



    template <typename T> struct is_like_pointer; // Forward declaration
    template <typename T> inline constexpr bool is_like_pointer_v = is_like_pointer<T>::value;

    template <typename T> concept    LikePointer =     is_like_pointer_v<T>;
    template <typename T> concept NotLikePointer = not is_like_pointer_v<T>;
    

    template <typename T>
    inline constexpr bool _is_any_void_ptr_v =
        ::std::is_void_v< ::std::remove_cv_t<::std::remove_pointer_t<::std::remove_reference_t<T>>> >;

    
    template <typename T>
    inline constexpr bool is_dereferenceable_v = ::std::is_pointer_v<T> and not _is_any_void_ptr_v<T>;

    template <Class C>
        requires(requires(C o) { o.operator*(); })
    inline constexpr bool is_dereferenceable_v<C> =
        NotSame<
            ::std::remove_cvref_t<C>,
            ::std::remove_cvref_t<decltype(std::declval<C>().operator*())>
        >;

    template <typename T> concept    Dereferenceable =     is_dereferenceable_v<T>;
    template <typename T> concept NotDereferenceable = not is_dereferenceable_v<T>;



    template <Dereferenceable T>
    using base_of_indirect_type = decltype(*std::declval<T>());



    template <typename T>
    inline constexpr bool has_arrow_v = false;

    template <Class C>
    inline constexpr bool has_arrow_v<C> =
        requires(C o)
        {
            o.operator->();
        };

    
    
    template <typename T>
        requires(has_arrow_v<T>)
    using arrow_result_t = decltype(std::declval<T>().operator->());




    template <typename T>
    struct is_like_pointer : ::std::false_type {};
    
    template <Pointer P>
    struct is_like_pointer<P> : ::std::true_type {};

    template <Class C>
        requires(has_arrow_v<C> and not is_dereferenceable_v<C>)
    struct is_like_pointer<C> : ::std::bool_constant<_is_any_void_ptr_v<arrow_result_t<C>>> {};

    template <Class C>
        requires(has_arrow_v<C> and is_dereferenceable_v<C>)
    struct is_like_pointer<C> : ::std::bool_constant<
            LikePointer<arrow_result_t<C>>
        and ::std::is_reference_v<base_of_indirect_type<C>>
        and Same<
                base_of_indirect_type<C>,
                base_of_indirect_type<arrow_result_t<C>>
            >
    > {};

    template <Class C>
        requires(not has_arrow_v<C> and is_dereferenceable_v<C>)
    struct is_like_pointer<C> : ::std::bool_constant<
            LikeFunction<base_of_indirect_type<C>>
        and ::std::is_reference_v<base_of_indirect_type<C>>
    > {};


    // ----------------------------------------------------------------------------------- array

    template <typename T> inline constexpr bool is_array_v = ::std::is_array_v<T>;

    template <typename T> concept    Array =     is_array_v<T>;
    template <typename T> concept NotArray = not is_array_v<T>;



    // ----------------------------------------------------------------------------------- char

    template <typename T>
    inline constexpr bool is_char_v = is_any_of_v< ::std::remove_const_t<T>, char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t >;

    template <typename T> concept    Char =     is_char_v<T>;
    template <typename T> concept NotChar = not is_char_v<T>;



    // ----------------------------------------------------------------------------------- is_basic_string

    template <typename T>
    struct is_basic_string
    {
    private:
        static consteval void is_basic_string_test(...) {};

        template <typename Ch, typename Tr, typename Al>
        static consteval auto is_basic_string_test(std::basic_string<Ch, Tr, Al> const& arg) { return arg; };
    protected:
        using as_basic_string_type = decltype(is_basic_string_test(std::declval<T>()));
    public:
        static constexpr bool value = not ::std::is_same_v<as_basic_string_type, void>;
    };

    template <typename T>
    inline constexpr bool is_basic_string_v = is_basic_string<T>::value;

    template <typename T> concept    BasicString =     is_basic_string_v<T>;
    template <typename T> concept NotBasicString = not is_basic_string_v<T>;

    template <BasicString BS>
    struct as_basic_string : is_basic_string<BS>
    {
        using type = is_basic_string<BS>::as_basic_string_type;
    };

    template <BasicString BS>
    using as_basic_string_t = typename as_basic_string<BS>::type;



    // ----------------------------------------------------------------------------------- like_string

    template <typename T>
    inline constexpr bool is_like_string_v = false;

    template <Container T>
        requires (is_char_v<::std::remove_cvref_t<container_elem_t<T>>>)
    inline constexpr bool is_like_string_v<T> = true;

    template <typename T> concept    LikeString =     is_like_string_v<T>;
    template <typename T> concept NotLikeString = not is_like_string_v<T>;


    // ----------------------------------------------------------------------------------- no duplicates

    template <typename T, typename... Ts>
    inline constexpr bool no_duplicates_v =
        not is_any_of_v<T, Ts...>
        and no_duplicates_v<Ts...>
    ;

    template <typename T>
    inline constexpr bool no_duplicates_v<T> = true;

    template <template <typename...> typename TsTmpl, typename... Ts>
        requires(std::is_base_of_v<types_container, TsTmpl<>>)
    inline constexpr bool no_duplicates_v<TsTmpl<Ts...>> = no_duplicates_v<Ts...>;

    template <template <typename...> typename TsTmpl, typename T>
        requires(std::is_base_of_v<types_container, TsTmpl<>>)
    inline constexpr bool no_duplicates_v<TsTmpl<T>> = true;


    // ----------------------------------------------------------------------------------- unique

    template <typename T, typename... Ts>
    inline constexpr bool is_unique_v =
        not (is_convertible_to_fromanyof_v<T, Ts...> or is_convertible_from_toanyof_v<T, Ts...>)
        and  is_unique_v<Ts...>
    ;

    template <typename T>
    inline constexpr bool is_unique_v<T> = true;

    template <template <typename...> typename TsTmpl, typename... Ts>
        requires(std::is_base_of_v<types_container, TsTmpl<>>)
    inline constexpr bool is_unique_v<TsTmpl<Ts...>> = is_unique_v<Ts...>;

    template <template <typename...> typename TsTmpl, typename T>
        requires(std::is_base_of_v<types_container, TsTmpl<>>)
    inline constexpr bool is_unique_v<TsTmpl<T>> = true;



    // ----------------------------------------------------------------------------------- sorted

    template <typename...>
    inline constexpr bool is_sorted_v = false;

    template <>
    inline constexpr bool is_sorted_v<> = true;

    template <typename T>
    inline constexpr bool is_sorted_v<T> = true;

    template <typename A, typename B, typename... Ts>
    inline constexpr bool is_sorted_v<A, B, Ts...> = not type_more_v<A, B> and is_sorted_v<B, Ts...>;

    template <template <typename...> typename TsTmpl, typename... Ts>
        requires(std::is_base_of_v<types_container, TsTmpl<>>)
    inline constexpr bool is_sorted_v<TsTmpl<Ts...>> = is_sorted_v<Ts...>;

    template <template <typename...> typename TsTmpl, typename T>
        requires(std::is_base_of_v<types_container, TsTmpl<>>)
    inline constexpr bool is_sorted_v<TsTmpl<T>> = true;



    // ----------------------------------------------------------------------------------- containers of types


    // INTERFACE

        template <typename...> struct types_pack;
        template <typename...> struct types_list;

        template <typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_info;

        template <typename...> struct types_first;
        template <typename...> struct types_last;

        template <typename, typename> struct types_concat;
        
        template <::std::size_t, typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_head;
        template <::std::size_t, typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_tail;

        template <typename TS, typename RT> requires(std::is_base_of_v<types_container, TS>) struct types_erase;

        template <typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_merge_sort;
        template <typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_quick_sort;

    // USING & CONST

        template <typename... Ts> using types_first_t = typename types_first<Ts...>::type;
        template <typename... Ts> using types_last_t  = typename types_last <Ts...>::type;

        template <typename A, typename B> using types_concat_t = types_concat<A, B>::type;

        template <size_t N, typename TS> using types_head_t = typename types_head<N, TS>::type;
        template <size_t N, typename TS> using types_tail_t = typename types_tail<N, TS>::type;

        template <typename TS, typename RT> using types_erase_t = typename types_erase<TS, RT>::type;

        template <typename TS> using types_merge_sort_t = typename types_merge_sort <TS>::type;
        template <typename TS> using types_quick_sort_t = typename types_quick_sort <TS>::type;

    // IMPLEMENTATION

    // ------------------------------------------------------------------------------- pack

    template <typename... Ts> struct types_pack : types_container
    {
        static constexpr size_t count() { return sizeof...(Ts); };
    };

    template <typename T>
    using get_types_pack = instantiate_like_t<types_pack, T>;


    // ------------------------------------------------------------------------------- list

    template <>
    struct types_list<> : types_container
    {
        static constexpr size_t count() { return 0; };
    };

    template <typename F, typename... Ts>
    struct types_list<F, Ts...> : types_list<Ts...>
    {
        static constexpr size_t count() { return sizeof...(Ts) + 1; };
    };

    template <typename T>
    using get_types_list = instantiate_like_t<types_list, T>;



    // ------------------------------------------------------------------------------- info

    template <template <typename...> typename TsTmpl>
    struct types_info<TsTmpl<>>
    {
    protected:

        template <size_t HeadCount, typename... Prev> requires(HeadCount == 0)
        using get_head = TsTmpl<Prev...>;

        template <size_t HeadCount> requires(HeadCount == 0)
        using get_tail = TsTmpl<>;

    public:

        using type = TsTmpl<>;

        static constexpr size_t count = 0;

        template <size_t N> requires(N == 0)
        using head = TsTmpl<>;

        template <size_t N> requires(N == 0)
        using tail = TsTmpl<>;

    };

    template <template <typename...> typename TsTmpl, typename First, typename... Rest>
    struct types_info<TsTmpl<First, Rest...>> : types_info<TsTmpl<Rest...>>
    {
    public:

        using type = TsTmpl<First, Rest...>;

    protected:

        template <size_t HeadCount, typename... Prev>
        using get_head = ::std::conditional_t<
            (HeadCount == 0)
            , TsTmpl<Prev...>
            , typename types_info<TsTmpl<Rest...>>::template get_head<HeadCount - 1, Prev..., First>
        >;

        template <size_t HeadCount>
        using get_tail = ::std::conditional_t <
            (HeadCount == 0)
            , type
            , typename types_info<TsTmpl<Rest...>>::template get_tail<HeadCount - 1>
        >;

    public:

        static constexpr size_t count = sizeof...(Rest) + 1;

        using first = types_first_t<type>;
        using last  = types_last_t <type>;

        template <size_t N>
        using head = get_head<N>;

        template <size_t N>
        using tail = get_tail<count - N>;

    };



    // ------------------------------------------------------------------------------- first

    template <typename T, typename... Ts>
    struct types_first<T, Ts...>
    {
        using type = T;
    };

    template <template <typename...> typename TsTmpl, typename T, typename... Ts>
    struct types_first<TsTmpl<T, Ts...>>
    {
        using type = T;
    };



    // ------------------------------------------------------------------------------- last

    template<typename... Ts>
    struct types_last
    {
        using type = typename decltype((type_tag<Ts>{}, ...))::type;
    };

    template <template <typename...> typename TsTmpl, typename... Ts>
    struct types_last<TsTmpl<Ts...>>
    {
        using type = typename decltype((type_tag<Ts>{}, ...))::type;
    };



    // ------------------------------------------------------------------------------- summ

    template <template <typename...> typename TsTmpl, typename... Ts1, typename... Ts2>
        requires(std::is_base_of_v<types_container, TsTmpl<>>)
    struct types_concat<TsTmpl<Ts1...>, TsTmpl<Ts2...>>
    {
        using type = TsTmpl<Ts1..., Ts2...>;
    };



    // ------------------------------------------------------------------------------- head

    template <size_t N, template <typename...> typename Tmpl, typename... Ts>
    struct types_head<N, Tmpl<Ts...>>
    {
    private:
        template <size_t I, typename, typename> struct impl;

        template <typename B, typename E>
        struct impl<0, B, E>
        {
            using res = B;
        };

        template <size_t I, typename... L, typename M, typename... R>
            requires (I > 0)
        struct impl<I, Tmpl<L...>, Tmpl<M, R...>>
        {
            using res = impl<I - 1, Tmpl<L..., M>, Tmpl<R...>>::res;
        };
    public:
        using type = impl<N, Tmpl<>, Tmpl<Ts...>>::res;
    };



    // ------------------------------------------------------------------------------- tail

    template <size_t N, template <typename...> typename Tmpl, typename First, typename... Rest>
        requires (N < (sizeof...(Rest) + 1))
    struct types_tail<N, Tmpl<First, Rest...>>
    {
        using type = typename types_tail<N, Tmpl<Rest...>>::type;
    };

    template <size_t N, template <typename...> typename Tmpl, typename... Ts>
        requires (N == sizeof...(Ts))
    struct types_tail<N, Tmpl<Ts...>>
    {
        using type = Tmpl<Ts...>;
    };

    template <template <typename...> typename Tmpl, typename... Ts>
    struct types_tail<0, Tmpl<Ts...>>
    {
        using type = Tmpl<>;
    };



    // ----------------------------------------------------------------------------------- remove

    template <typename RT, template <typename...> typename TsTmpl>
    struct types_erase<TsTmpl<>, RT>
    {
        using type = TsTmpl<>;
    };

    template <typename RT, template <typename...> typename TsTmpl, typename F, typename... Ts>
    struct types_erase<TsTmpl<F, Ts...>, RT>
    {
        using type = types_concat_t<TsTmpl<F>, typename types_erase<TsTmpl<Ts...>, RT>::type>;
    };

    template <typename RT, template <typename...> typename TsTmpl, typename... Ts>
    struct types_erase<TsTmpl<RT, Ts...>, RT>
    {
        using type = typename types_erase<TsTmpl<Ts...>, RT>::type;
    };



    // ----------------------------------------------------------------------------------- types sort

    // merge sort

    // https://stackoverflow.com/a/64795244/23601704
    template <template <typename...> typename TsTmpl, typename... Types>
    struct types_merge_sort<TsTmpl<Types...>>
    {
    private:

        template <typename, typename> struct merge {};
        template <typename TL1, typename TL2> using  merge_t = typename merge<TL1, TL2>::type;

        template <typename... Ts>
        struct merge<TsTmpl<>, TsTmpl<Ts...>>
        {
            using type = TsTmpl<Ts...>;
        };

        template <typename... Ts>
        struct merge<TsTmpl<Ts...>, TsTmpl<>>
        {
            using type = TsTmpl<Ts...>;
        };

        template <typename A, typename... As, typename B, typename... Bs>
        struct merge<TsTmpl<A, As...>, TsTmpl<B, Bs...>>
        {
            using type = ::std::conditional_t<
                type_less_v<A, B>,
                types_concat_t<TsTmpl<A>, merge_t<TsTmpl<   As...>, TsTmpl<B, Bs...>>>,
                types_concat_t<TsTmpl<B>, merge_t<TsTmpl<A, As...>, TsTmpl<   Bs...>>>
            >;
        };



        template <typename   > struct sort {};
        template <typename TS> using  sort_t = typename sort<TS>::type;

        template <sib::Same<TsTmpl<>> T>
        struct sort<T>
        {
            using type = TsTmpl<>;
        };

        template <typename T>
        struct sort<TsTmpl<T>>
        {
            using type = TsTmpl<T>;
        };

        template <typename A, typename B>
        struct sort<TsTmpl<A, B>>
        {
            using type = ::std::conditional_t<type_less_v<A, B>, TsTmpl<A, B>, TsTmpl<B, A>>;
        };

        template <typename... Ts>
            requires(sizeof...(Ts) > 2)
        struct sort<TsTmpl<Ts...>>
        {
            static constexpr ::std::size_t middle = sizeof...(Ts) / 2;
            using type = merge_t<
                sort_t<types_head_t<                middle, TsTmpl<Ts...>>>,
                sort_t<types_tail_t<sizeof...(Ts) - middle, TsTmpl<Ts...>>>
            >;
        };

    public:
        using type = sort_t<TsTmpl<Types...>>;
    };



    // quick sort

    template <template <typename...> typename TsTmpl, typename... Types>
    struct types_quick_sort<TsTmpl<Types...>>
    {
    private:

        struct PASS {};
        
        template <typename Cond, typename... Ts>
        using left  = types_erase_t<TsTmpl< ::std::conditional_t< type_less_v<Ts, Cond>, Ts, PASS> ... >, PASS>;

        template <typename Cond, typename... Ts>
        using right = types_erase_t<TsTmpl< ::std::conditional_t<!type_less_v<Ts, Cond>, Ts, PASS> ... >, PASS>;



        template <typename...> struct Ls_M_Rs;

        template <typename M, typename... Ls, typename... Rs>
        struct Ls_M_Rs<TsTmpl<Ls...>, M, TsTmpl<Rs...>>
        {
            using res = TsTmpl<Ls..., M, Rs...>;
        };



        template <typename   > struct sort {};
        template <typename TS> using  sort_t = typename sort<TS>::type;

        template <sib::Same<TsTmpl<>> EmptyTs>
        struct sort<EmptyTs>
        {
            using type = TsTmpl<>;
        };

        template <typename F, typename... Ts>
        struct sort<TsTmpl<F, Ts...>>
        {
            using type = typename Ls_M_Rs<
                sort_t<left <F, Ts...>>,
                F,
                sort_t<right<F, Ts...>>
            >::res;
        };

    public:
        using type = sort_t<TsTmpl<Types...>>;
    };

} // namespace sib
