#pragma once

#include <type_traits>

namespace sib {

    // ----------------------------------------------------------------------------------- ADVANCE DECLARATIONS

    struct types_container  // base type for containers of types
    {
        static constexpr size_t count();
    };

    // ----------------------------------------------------------------------------------- static type name

    /* https://stackoverflow.com/a/64490578/23601704 */

    namespace detail {

        template <typename T>
        constexpr std::string_view SIB_STN() noexcept
        {
            #ifdef __clang__
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
    constexpr std::string_view static_type_name() noexcept
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



    // ----------------------------------------------------------------------------------- compare types

    template <typename L, typename R>
    constexpr bool type_equal_v = std::is_same_v<L, R>;

    template <typename L, typename R>
    constexpr bool type_less_v = (sib::static_type_name<L>() < sib::static_type_name<R>());

    template <typename L, typename R>
    constexpr bool type_more_v = (sib::static_type_name<L>() > sib::static_type_name<R>());



    // ----------------------------------------------------------------------------------- instantiation of template

    template <typename T, template <typename...> typename Templ>
    constexpr bool is_instantiation_of_v = false;

    template <template <typename...> typename Templ, typename... Ts>
    constexpr bool is_instantiation_of_v<Templ<Ts...>, Templ> = true;

    template <typename T, template <typename...> typename Templ>
    concept    InstantiationOf =     is_instantiation_of_v<T, Templ>;
    
    template <typename T, template <typename...> typename Templ>
    concept NotInstantiationOf = not is_instantiation_of_v<T, Templ>;

    
    template <template <typename...> typename Templ, typename... Ts>
    struct instantiate_templ
    {
        using type = Templ<Ts...>;
    };

    template <template <typename...> typename Templ, template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<types_container, TsTempl<>>)
    struct instantiate_templ<Templ, TsTempl<Ts...>>
    {
        using type = Templ<Ts...>;
    };

    template <template <typename...> typename Templ, typename... Ts>
    using instantiate_templ_t = typename instantiate_templ<Templ, Ts...>::type;
    
    
    
    template <template <typename...> typename Templ, typename Like>
    struct instantiate_like
    {
        static_assert(always_false_v<Like>, "Template parameter Like is not instantiate of template.");
    };
    
    template <template <typename...> typename Templ, template <typename...> typename Like, typename... Ts>
    struct instantiate_like<Templ, Like<Ts...>>
    {
        using type = Templ<Ts...>;
    };
    
    template <template <typename...> typename Templ, typename Like>
    using instantiate_like_t = typename instantiate_like<Templ, Like>::type;

    // ----------------------------------------------------------------------------------- conditional_v

    template <bool Test, auto Val1, auto Val2 = decltype(Val1){}>
    constexpr auto conditional_v = Val1;

    template <auto Val1, auto Val2>
    constexpr auto conditional_v<false, Val1, Val2> = Val2;

    // ----------------------------------------------------------------------------------- is same

    template <typename T, typename First, typename... Rest>
    constexpr bool is_any_of_v = std::is_same_v<T, First> or (std::is_same_v<T, Rest> or ...);

    #if 1
        template <typename T, typename First, typename... Rest>
        constexpr bool is_one_of_v =
            std::is_same_v<T, First>
            ? not (std::is_same_v<T, Rest> or ...)
            : is_one_of_v<T, Rest...>
        ;

        template <typename T, typename First>
        constexpr bool is_one_of_v <T, First> = std::is_same_v<T, First>;
    #else
        template <typename T, typename... Ts>
        inline consteval bool one_of_test()
        {
            size_t counter = 0;
            return ((counter == 2 ? 2 : (counter += std::is_same_v<T, Ts>)), ...) == 1;
        }

        template <typename T, typename... Ts>
        constexpr bool is_one_of_v = one_of_test<T, Ts...>();
    #endif

    template <typename T, typename    U > concept    Same  =     std::is_same_v  <T, U    >;
    template <typename T, typename... Ts> concept    AnyOf =          is_any_of_v<T, Ts...>;
    template <typename T, typename... Ts> concept    OneOf =          is_one_of_v<T, Ts...>;

    template <typename T, typename    U > concept NotSame  = not std::is_same_v  <T, U    >;
    template <typename T, typename... Ts> concept NotOneOf = not      is_one_of_v<T, Ts...>;
    template <typename T, typename... Ts> concept NotAnyOf = not      is_any_of_v<T, Ts...>;



    // ----------------------------------------------------------------------------------- constract/convert/cast

            template <typename, typename...> struct construct_from_tooneof;
            template <typename, typename...> struct   convert_from_tooneof;
            template <typename, typename...> struct      cast_from_tooneof;
            template <typename, typename...> struct construct_to_fromoneof;
            template <typename, typename...> struct   convert_to_fromoneof;
            template <typename, typename...> struct      cast_to_fromoneof;

    template <typename From, typename To> constexpr bool is_constructible_from_to_v = std::is_constructible_v<To, From>;
    template <typename From, typename To> constexpr bool   is_convertible_from_to_v = std::  is_convertible_v<From, To>;
    template <typename From, typename To> constexpr bool      is_castable_from_to_v = requires { static_cast<To>(std::declval<From>()); };
    template <typename To, typename From> constexpr bool is_constructible_to_from_v = std::is_constructible_v<To, From>;
    template <typename To, typename From> constexpr bool   is_convertible_to_from_v = std::is_convertible_v<From, To>;
    template <typename To, typename From> constexpr bool      is_castable_to_from_v = requires { static_cast<To>(std::declval<From>()); };

    template <typename From, typename... To> constexpr bool is_constructible_from_toanyof_v = (is_constructible_from_to_v<From, To> or ...);
    template <typename From, typename... To> constexpr bool   is_convertible_from_toanyof_v = (  is_convertible_from_to_v<From, To> or ...);
    template <typename From, typename... To> constexpr bool      is_castable_from_toanyof_v = (     is_castable_from_to_v<From, To> or ...);
    template <typename To, typename... From> constexpr bool is_constructible_to_fromanyof_v = (is_constructible_to_from_v<To, From> or ...);
    template <typename To, typename... From> constexpr bool   is_convertible_to_fromanyof_v = (  is_convertible_to_from_v<To, From> or ...);
    template <typename To, typename... From> constexpr bool      is_castable_to_fromanyof_v = (     is_castable_to_from_v<To, From> or ...);

    template <typename From, typename To, typename...   To_other> constexpr bool is_constructible_from_tooneof_v = construct_from_tooneof<From, To,   To_other...>::found;
    template <typename From, typename To, typename...   To_other> constexpr bool   is_convertible_from_tooneof_v =   convert_from_tooneof<From, To,   To_other...>::found;
    template <typename From, typename To, typename...   To_other> constexpr bool      is_castable_from_tooneof_v =      cast_from_tooneof<From, To,   To_other...>::found;
    template <typename To, typename From, typename... From_other> constexpr bool is_constructible_to_fromoneof_v = construct_to_fromoneof<To, From, From_other...>::found;
    template <typename To, typename From, typename... From_other> constexpr bool   is_convertible_to_fromoneof_v =   convert_to_fromoneof<To, From, From_other...>::found;
    template <typename To, typename From, typename... From_other> constexpr bool      is_castable_to_fromoneof_v =      cast_to_fromoneof<To, From, From_other...>::found;

    template <typename From, typename    To> concept    ConstructibleFromTo      =     is_constructible_from_to_v     <From, To   >;
    template <typename From, typename    To> concept      ConvertibleFromTo      =       is_convertible_from_to_v     <From, To   >;
    template <typename From, typename    To> concept         CastableFromTo      =          is_castable_from_to_v     <From, To   >;
    template <typename From, typename... To> concept    ConstructibleFromToAnyOf =     is_constructible_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept      ConvertibleFromToAnyOf =       is_convertible_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept         CastableFromToAnyOf =          is_castable_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept    ConstructibleFromToOneOf =     is_constructible_from_tooneof_v<From, To...>;
    template <typename From, typename... To> concept      ConvertibleFromToOneOf =       is_convertible_from_tooneof_v<From, To...>;
    template <typename From, typename... To> concept         CastableFromToOneOf =          is_castable_from_tooneof_v<From, To...>;
    template <typename To, typename    From> concept    ConstructibleToFrom      =     is_constructible_to_from_v     <To, From   >;
    template <typename To, typename    From> concept      ConvertibleToFrom      =       is_convertible_to_from_v     <To, From   >;
    template <typename To, typename    From> concept         CastableToFrom      =          is_castable_to_from_v     <To, From   >;
    template <typename To, typename... From> concept    ConstructibleToFromAnyOf =     is_constructible_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept      ConvertibleToFromAnyOf =       is_convertible_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept         CastableToFromAnyOf =          is_castable_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept    ConstructibleToFromOneOf =     is_constructible_to_fromoneof_v<To, From...>;
    template <typename To, typename... From> concept      ConvertibleToFromOneOf =       is_convertible_to_fromoneof_v<To, From...>;
    template <typename To, typename... From> concept         CastableToFromOneOf =          is_castable_to_fromoneof_v<To, From...>;

    template <typename From, typename    To> concept NotConstructibleFromTo      = not is_constructible_from_to_v     <From, To   >;
    template <typename From, typename    To> concept   NotConvertibleFromTo      = not   is_convertible_from_to_v     <From, To   >;
    template <typename From, typename    To> concept      NotCastableFromTo      = not      is_castable_from_to_v     <From, To   >;
    template <typename From, typename... To> concept NotConstructibleFromToAnyOf = not is_constructible_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept   NotConvertibleFromToAnyOf = not   is_convertible_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept      NotCastableFromToAnyOf = not      is_castable_from_toanyof_v<From, To...>;
    template <typename From, typename... To> concept NotConstructibleFromToOneOf = not is_constructible_from_tooneof_v<From, To...>;
    template <typename From, typename... To> concept   NotConvertibleFromToOneOf = not   is_convertible_from_tooneof_v<From, To...>;
    template <typename From, typename... To> concept      NotCastableFromToOneOf = not      is_castable_from_tooneof_v<From, To...>;
    template <typename To, typename    From> concept NotConstructibleToFrom      = not is_constructible_to_from_v     <To, From   >;
    template <typename To, typename    From> concept   NotConvertibleToFrom      = not   is_convertible_to_from_v     <To, From   >;
    template <typename To, typename    From> concept      NotCastableToFrom      = not      is_castable_to_from_v     <To, From   >;
    template <typename To, typename... From> concept NotConstructibleToFromAnyOf = not is_constructible_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept   NotConvertibleToFromAnyOf = not   is_convertible_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept      NotCastableToFromAnyOf = not      is_castable_to_fromanyof_v<To, From...>;
    template <typename To, typename... From> concept NotConstructibleToFromOneOf = not is_constructible_to_fromoneof_v<To, From...>;
    template <typename To, typename... From> concept   NotConvertibleToFromOneOf = not   is_convertible_to_fromoneof_v<To, From...>;
    template <typename To, typename... From> concept      NotCastableToFromOneOf = not      is_castable_to_fromoneof_v<To, From...>;

    template <typename From, typename... To> requires(is_constructible_from_tooneof_v<From, To...>) using construct_from_tooneof_select = typename construct_from_tooneof<From, To...>::select;
    template <typename From, typename... To> requires(  is_convertible_from_tooneof_v<From, To...>) using   convert_from_tooneof_select = typename   convert_from_tooneof<From, To...>::select;
    template <typename From, typename... To> requires(     is_castable_from_tooneof_v<From, To...>) using      cast_from_tooneof_select = typename      cast_from_tooneof<From, To...>::select;
    template <typename To, typename... From> requires(is_constructible_to_fromoneof_v<To, From...>) using construct_to_fromoneof_select = typename construct_to_fromoneof<To, From...>::select;
    template <typename To, typename... From> requires(  is_convertible_to_fromoneof_v<To, From...>) using   convert_to_fromoneof_select = typename   convert_to_fromoneof<To, From...>::select;
    template <typename To, typename... From> requires(     is_castable_to_fromoneof_v<To, From...>) using      cast_to_fromoneof_select = typename      cast_to_fromoneof<To, From...>::select;

            #define SIB_CCC_MAC(F, S, f, s, ACTION, CHARACT)                                        \
                template <typename F, typename S, typename... S##_other>                            \
                struct ACTION##_##f##_##s##oneof<F, S, S##_other...>                                \
                {                                                                                   \
                    static constexpr bool found = false;                                            \
                };                                                                                  \
                                                                                                    \
                template <typename F, typename S, typename... S##_other>                            \
                    requires (     (    is_##CHARACT##_##f##_##s##_v     <F, S           >)         \
                               and (not is_##CHARACT##_##f##_##s##anyof_v<F, S##_other...>) )       \
                struct ACTION##_##f##_##s##oneof<F, S, S##_other...>                                \
                {                                                                                   \
                    static constexpr bool found = true;                                             \
                    using select = S;                                                               \
                };                                                                                  \
                                                                                                    \
                template <typename F, typename S, typename... S##_other>                            \
                    requires (     (not is_##CHARACT##_##f##_##s##_v     <F, S           >)         \
                               and (    is_##CHARACT##_##f##_##s##oneof_v<F, S##_other...>) )       \
                struct ACTION##_##f##_##s##oneof<F, S, S##_other...>                                \
                {                                                                                   \
                    static constexpr bool found = true;                                             \
                    using select = ACTION##_##f##_##s##oneof_select<F, S##_other...>;               \
                };                                                                                  \

            SIB_CCC_MAC(From, To, from, to, construct, constructible)
            SIB_CCC_MAC(From, To, from, to,   convert,   convertible)
            SIB_CCC_MAC(From, To, from, to,      cast,      castable)
            SIB_CCC_MAC(To, From, to, from, construct, constructible)
            SIB_CCC_MAC(To, From, to, from,   convert,   convertible)
            SIB_CCC_MAC(To, From, to, from,      cast,      castable)



    // ----------------------------------------------------------------------------------- assign

            template <typename, typename, typename...> struct assign_from_tooneof;
            template <typename, typename, typename...> struct assign_to_fromoneof;

    template <typename Source, typename    Dest> constexpr bool is_assignable_from_to_v      = std::is_assignable_v<Dest, Source>;
    template <typename Dest, typename    Source> constexpr bool is_assignable_to_from_v      = std::is_assignable_v<Dest, Source>;

    template <typename Source, typename... Dest> constexpr bool is_assignable_from_toanyof_v = (is_assignable_from_to_v<Source, Dest> or ...);
    template <typename Dest, typename... Source> constexpr bool is_assignable_to_fromanyof_v = (is_assignable_from_to_v<Dest, Source> or ...);

    template <typename Source, typename Dest, typename...   Dest_other> constexpr bool is_assignable_from_tooneof_v = assign_from_tooneof<Source, Dest,   Dest_other...>::found;
    template <typename Dest, typename Source, typename... Source_other> constexpr bool is_assignable_to_fromoneof_v = assign_from_tooneof<Dest, Source, Source_other...>::found;

    template <typename Source, typename    Dest> concept    AssignableFromTo      =     is_assignable_from_to_v     <Source, Dest   >;
    template <typename Source, typename... Dest> concept    AssignableFromToAnyOf =     is_assignable_from_toanyof_v<Source, Dest...>;
    template <typename Source, typename... Dest> concept    AssignableFromToOneOf =     is_assignable_from_tooneof_v<Source, Dest...>;
    template <typename Dest, typename    Source> concept    AssignableToFrom      =     is_assignable_to_from_v     <Dest, Source   >;
    template <typename Dest, typename... Source> concept    AssignableToFromAnyOf =     is_assignable_to_fromanyof_v<Dest, Source...>;
    template <typename Dest, typename... Source> concept    AssignableToFromOneOf =     is_assignable_to_fromoneof_v<Dest, Source...>;

    template <typename Source, typename    Dest> concept NotAssignableFromTo      = not is_assignable_from_to_v     <Source, Dest   >;
    template <typename Source, typename... Dest> concept NotAssignableFromToAnyOf = not is_assignable_from_toanyof_v<Source, Dest...>;
    template <typename Source, typename... Dest> concept NotAssignableFromToOneOf = not is_assignable_from_tooneof_v<Source, Dest...>;
    template <typename Dest, typename    Source> concept NotAssignableToFrom      = not is_assignable_to_from_v     <Dest, Source   >;
    template <typename Dest, typename... Source> concept NotAssignableToFromAnyOf = not is_assignable_to_fromanyof_v<Dest, Source...>;
    template <typename Dest, typename... Source> concept NotAssignableToFromOneOf = not is_assignable_to_fromoneof_v<Dest, Source...>;


    template <typename Source, typename Dest> requires(is_assignable_from_to_v<Source, Dest>) using assign_from_to_result = decltype(std::declval<Dest>() = std::declval<Source>());
    template <typename Dest, typename Source> requires(is_assignable_to_from_v<Dest, Source>) using assign_to_from_result = decltype(std::declval<Dest>() = std::declval<Source>());

    template <typename Source, typename... Dest> requires(is_assignable_from_tooneof_v<Source, Dest...>) using assign_from_tooneof_select = typename assign_from_tooneof<Source, Dest...>::select;
    template <typename Source, typename... Dest> requires(is_assignable_from_tooneof_v<Source, Dest...>) using assign_from_tooneof_result = typename assign_from_tooneof<Source, Dest...>::result;
    template <typename Dest, typename... Source> requires(is_assignable_to_fromoneof_v<Dest, Source...>) using assign_to_fromoneof_select = typename assign_to_fromoneof<Dest, Source...>::select;
    template <typename Dest, typename... Source> requires(is_assignable_to_fromoneof_v<Dest, Source...>) using assign_to_fromoneof_result = typename assign_to_fromoneof<Dest, Source...>::result;

            #define SIB_ASSIGN_MAC(F, S, f, s)                                                      \
                template <typename F, typename S, typename... S##_other>                            \
                struct assign_##f##_##s##oneof<F, S, S##_other...>                                  \
                {                                                                                   \
                    static constexpr bool found = false;                                            \
                };                                                                                  \
                                                                                                    \
                template <typename F, typename S, typename... S##_other>                            \
                    requires (     (    is_assignable_##f##_##s##_v     <F, S           >)          \
                               and (not is_assignable_##f##_##s##anyof_v<F, S##_other...>) )        \
                struct assign_##f##_##s##oneof<F, S, S##_other...>                                  \
                {                                                                                   \
                    static constexpr bool found = true;                                             \
                    using select = S;                                                               \
                    using result = assign_##f##_##s##_result<F, S>;                                 \
                };                                                                                  \
                                                                                                    \
                template <typename F, typename S, typename... S##_other>                            \
                    requires (     (not is_assignable_##f##_##s##_v     <F, S           >)          \
                               and (    is_assignable_##f##_##s##oneof_v<F, S##_other...>) )        \
                struct assign_##f##_##s##oneof<F, S, S##_other...>                                  \
                {                                                                                   \
                    static constexpr bool found = true;                                             \
                    using select = assign_##f##_##s##oneof_select<F, S##_other...>;                 \
                    using result = assign_##f##_##s##oneof_result<F, S##_other...>;                 \
                };                                                                                  \

            SIB_ASSIGN_MAC(Source, Dest, from, to)
            SIB_ASSIGN_MAC(Dest, Source, to, from)

    // ----------------------------------------------------------------------------------- container

    template <typename T>
    constexpr bool is_container_v = // requires (T v) { for (auto it : v) {} } // до лучших времён
        requires(T & v)
        {
            { std::begin(v) == std::end(v) } -> std::same_as<bool>;
            requires requires(decltype(std::begin(v)) it)
            {
                { *it };
                { ++it } -> std::same_as<decltype(it)&>;
            };
        }
    ;

    template <typename T> concept    Container =     is_container_v<T>;
    template <typename T> concept NotContainer = not is_container_v<T>;

    template <Container Cont>
    using container_elem_t = decltype(*std::begin(std::declval<Cont&>()));



    // ----------------------------------------------------------------------------------- enum

    template <typename T> constexpr bool is_enum_v = std::is_enum_v<T>;

    template <typename T> concept    Enum =     is_enum_v<T>;
    template <typename T> concept NotEnum = not is_enum_v<T>;



    template <typename T>
    struct is_enum_class : std::false_type {};

    template <Enum E> requires (not std::is_convertible_v<E, std::underlying_type_t<std::remove_cvref_t<E>>>)
    struct is_enum_class<E> : std::true_type {};

    template <typename T> constexpr bool  is_enum_class_v = is_enum_class<T>::value;

    template <typename T> concept    EnumClass =     is_enum_class_v<T>;
    template <typename T> concept NotEnumClass = not is_enum_class_v<T>;



    // ----------------------------------------------------------------------------------- function

    template <typename T> constexpr bool is_function_v = std::is_function_v<T>;

    template <typename T> concept    Function =     is_function_v<T>;
    template <typename T> concept NotFunction = not is_function_v<T>;



    template <typename T>
    constexpr bool is_like_function_v = requires (T f) { std::function(f); };

    template <typename T> concept    LikeFunction =     is_like_function_v<T>;
    template <typename T> concept NotLikeFunction = not is_like_function_v<T>;



    // ----------------------------------------------------------------------------------- pointer

    template <typename T> constexpr bool is_pointer_v = std::is_pointer_v<T>;

    template <typename T> concept    Pointer =     is_pointer_v<T>;
    template <typename T> concept NotPointer = not is_pointer_v<T>;



    template <typename T>
    constexpr bool is_like_pointer_v = false;

    template <typename T>
        requires
        (
                     is_pointer_v<T>
            or ( not is_function_v<T>
                 and is_convertible_from_to_v<T, void const*> )
            or (     is_like_pointer_v< decltype( std::declval<T>().operator->()) >
                 and std::is_same_v   < decltype( std::declval<T>().operator->()),
                                        decltype(&std::declval<T>().operator *()) > )
        )
    constexpr bool is_like_pointer_v<T> = true;

    template <typename T> concept    LikePointer =     is_like_pointer_v<T>;
    template <typename T> concept NotLikePointer = not is_like_pointer_v<T>;



    template <typename T>
    constexpr bool may_be_indirect_v =
                is_like_pointer_v<T>
        and not is_function_v<T>
        and     requires(T t) { *t; }
    ;

    template <typename T> concept    Indirected =     may_be_indirect_v<T>;
    template <typename T> concept NotIndirected = not may_be_indirect_v<T>;



    template <Indirected P>
    using base_indirection_type = std::remove_reference_t<decltype(*std::declval<P>())>;



    // ----------------------------------------------------------------------------------- array

    template <typename T> constexpr bool  is_array_v = std::is_array_v<T>;

    template <typename T> concept    Array =     is_array_v<T>;
    template <typename T> concept NotArray = not is_array_v<T>;



    // ----------------------------------------------------------------------------------- char

    template <typename T>
    constexpr bool is_char_v = is_any_of_v< std::remove_const_t<T>, char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t >;

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
        static constexpr bool value = not std::is_same_v<as_basic_string_type, void>;
    };

    template <typename T>
    constexpr bool is_basic_string_v = is_basic_string<T>::value;

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
    constexpr bool is_like_string_v = false;

    template <Container T>
        requires (is_char_v<std::remove_cvref_t<container_elem_t<T>>>)
    constexpr bool is_like_string_v<T> = true;

    template <typename T> concept    LikeString =     is_like_string_v<T>;
    template <typename T> concept NotLikeString = not is_like_string_v<T>;


    // ----------------------------------------------------------------------------------- no duplicates

    template <typename T, typename... Ts>
    constexpr bool no_duplicates_v =
        not is_any_of_v<T, Ts...>
        and no_duplicates_v<Ts...>
    ;

    template <typename T>
    constexpr bool no_duplicates_v<T> = true;

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<types_container, TsTempl<>>)
    constexpr bool no_duplicates_v<TsTempl<Ts...>> = no_duplicates_v<Ts...>;

    template <template <typename...> typename TsTempl, typename T>
        requires(std::is_base_of_v<types_container, TsTempl<>>)
    constexpr bool no_duplicates_v<TsTempl<T>> = true;


    // ----------------------------------------------------------------------------------- unique

    template <typename T, typename... Ts>
    constexpr bool is_unique_v =
        not (is_convertible_to_fromanyof_v<T, Ts...> or is_convertible_from_toanyof_v<T, Ts...>)
        and is_unique_v<Ts...>
    ;

    template <typename T>
    constexpr bool is_unique_v<T> = true;

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<types_container, TsTempl<>>)
    constexpr bool is_unique_v<TsTempl<Ts...>> = is_unique_v<Ts...>;

    template <template <typename...> typename TsTempl, typename T>
        requires(std::is_base_of_v<types_container, TsTempl<>>)
    constexpr bool is_unique_v<TsTempl<T>> = true;



    // ----------------------------------------------------------------------------------- sorted

    template <typename...>
    constexpr bool is_sorted_v = false;

    template <>
    constexpr bool is_sorted_v<> = true;

    template <typename T>
    constexpr bool is_sorted_v<T> = true;

    template <typename A, typename B, typename... Ts>
    constexpr bool is_sorted_v<A, B, Ts...> = not type_more_v<A, B> and is_sorted_v<B, Ts...>;

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<types_container, TsTempl<>>)
    constexpr bool is_sorted_v<TsTempl<Ts...>> = is_sorted_v<Ts...>;

    template <template <typename...> typename TsTempl, typename T>
        requires(std::is_base_of_v<types_container, TsTempl<>>)
    constexpr bool is_sorted_v<TsTempl<T>> = true;



    // ----------------------------------------------------------------------------------- containers of types


    // INTERFACE

        template <typename...> struct types_pack;
        template <typename...> struct types_list;

        template <typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_info;

        template <typename... Ts> struct types_first;
        template <typename... Ts> struct types_last;

        template <typename, typename> struct types_summ;
        
        template <size_t, typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_head;
        template <size_t, typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_tail;

        template <typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_merge_sort;
        template <typename TS> requires(std::is_base_of_v<types_container, TS>) struct types_quick_sort;

    // USING & CONST

        template <typename... Ts> using types_first_t = typename types_first<Ts...>::type;
        template <typename... Ts> using types_last_t  = typename types_last <Ts...>::type;

        template <typename A, typename B> using types_summ_t = types_summ<A, B>::type;

        template <size_t N, typename TS> using types_head_t = typename types_head<N, TS>::type;
        template <size_t N, typename TS> using types_tail_t = typename types_tail<N, TS>::type;

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

    template <template <typename...> typename TsTempl>
    struct types_info<TsTempl<>>
    {
    protected:

        template <size_t HeadCount, typename... Prev> requires(HeadCount == 0)
        using get_head = TsTempl<Prev...>;

        template <size_t HeadCount> requires(HeadCount == 0)
        using get_tail = TsTempl<>;

    public:

        using type = TsTempl<>;

        static constexpr size_t count = 0;

        template <size_t N> requires(N == 0)
        using head = TsTempl<>;

        template <size_t N> requires(N == 0)
        using tail = TsTempl<>;

    };

    template <template <typename...> typename TsTempl, typename First, typename... Rest>
    struct types_info<TsTempl<First, Rest...>> : types_info<TsTempl<Rest...>>
    {
    public:

        using type = TsTempl<First, Rest...>;

    protected:

        template <size_t HeadCount, typename... Prev>
        using get_head = std::conditional_t<
            (HeadCount == 0)
            , TsTempl<Prev...>
            , typename types_info<TsTempl<Rest...>>::template get_head<HeadCount - 1, Prev..., First>
        >;

        template <size_t HeadCount>
        using get_tail = std::conditional_t <
            (HeadCount == 0)
            , type
            , typename types_info<TsTempl<Rest...>>::template get_tail<HeadCount - 1>
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

    template <template <typename...> typename TsTempl, typename T, typename... Ts>
    struct types_first<TsTempl<T, Ts...>>
    {
        using type = T;
    };



    // ------------------------------------------------------------------------------- last

    template<typename... Ts>
    struct types_last
    {
        using type = typename decltype((type_tag<Ts>{}, ...))::type;
    };

    template <template <typename...> typename TsTempl, typename... Ts>
    struct types_last<TsTempl<Ts...>>
    {
        using type = typename decltype((type_tag<Ts>{}, ...))::type;
    };



    // ------------------------------------------------------------------------------- summ

    template <template <typename...> typename Templ, typename... Ts1, typename... Ts2>
        requires(std::is_base_of_v<types_container, Templ<>>)
    struct types_summ<Templ<Ts1...>, Templ<Ts2...>>
    {
        using type = Templ<Ts1..., Ts2...>;
    };

    template <typename T, template <typename...> typename Templ, typename... Ts>
        requires(
            not is_instantiation_of_v<T, Templ>
            and std::is_base_of_v<types_container, Templ<>>
        )
    struct types_summ<T, Templ<Ts...>>
    {
        using type = Templ<T, Ts...>;
    };

    template <typename T, template <typename...> typename Templ, typename... Ts>
        requires(
            not is_instantiation_of_v<T, Templ>
            and std::is_base_of_v<types_container, Templ<>>
        )
    struct types_summ<Templ<Ts...>, T>
    {
        using type = Templ<Ts..., T>;
    };



    // ------------------------------------------------------------------------------- head

    template <size_t N, template <typename...> typename Templ, typename... Ts>
    struct types_head<N, Templ<Ts...>>
    {
    private:
        template <size_t I, typename, typename> struct impl;

        template <typename B, typename E>
        struct impl<0, B, E>
        {
            using res = Templ<>;
        };

        template <size_t I, typename... L, typename M, typename... R>
        struct impl<I, Templ<L...>, Templ<M, R...>>
        {
            using res = impl<I - 1, Templ<L..., M>, Templ<R...>>::res;
        };
    public:
        using type = impl<N, Templ<>, Templ<Ts...>>::res;
    };



    // ------------------------------------------------------------------------------- tail

    template <size_t N, template <typename...> typename Templ, typename First, typename... Rest>
        requires (N < (sizeof...(Rest) + 1))
    struct types_tail<N, Templ<First, Rest...>>
    {
        using type = typename types_tail<N, Templ<Rest...>>::type;
    };

    template <size_t N, template <typename...> typename Templ, typename... Ts>
        requires (N == sizeof...(Ts))
    struct types_tail<N, Templ<Ts...>>
    {
        using type = Templ<Ts...>;
    };

    template <template <typename...> typename Templ, typename... Ts>
    struct types_tail<0, Templ<Ts...>>
    {
        using type = Templ<>;
    };



    // ----------------------------------------------------------------------------------- types sort

    // merge sort

    // https://stackoverflow.com/a/64795244/23601704
    template <template <typename...> typename TsTempl, typename... Types>
    struct types_merge_sort<TsTempl<Types...>>
    {
    private:

        template <typename, typename    > struct merge;
        template <typename TL1, typename TL2> using  merge_t = typename merge<TL1, TL2>::type;

        template <typename... Ts>
        struct merge<TsTempl<>, TsTempl<Ts...>>
        {
            using type = TsTempl<Ts...>;
        };

        template <typename... Ts>
        struct merge<TsTempl<Ts...>, TsTempl<>>
        {
            using type = TsTempl<Ts...>;
        };

        template <typename A, typename... As, typename B, typename... Bs>
        struct merge<TsTempl<A, As...>, TsTempl<B, Bs...>>
        {
            using type = std::conditional_t<
                type_less_v<A, B>,
                types_summ_t<TsTempl<A>, merge_t<TsTempl<   As...>, TsTempl<B, Bs...>>>,
                types_summ_t<TsTempl<B>, merge_t<TsTempl<A, As...>, TsTempl<   Bs...>>>
            >;
        };



        template <typename   > struct sort;
        template <typename TS> using  sort_t = typename sort<TS>::type;

        template <>
        struct sort<TsTempl<>>
        {
            using type = TsTempl<>;
        };

        template <typename T>
        struct sort<TsTempl<T>>
        {
            using type = TsTempl<T>;
        };

        template <typename A, typename B>
        struct sort<TsTempl<A, B>>
        {
            using type = std::conditional_t<type_less_v<A, B>, TsTempl<A, B>, TsTempl<B, A>>;
        };

        template <typename... Ts>
        struct sort<TsTempl<Ts...>>
        {
            static constexpr size_t middle = sizeof...(Ts) / 2;
            using type = merge_t<
                sort_t<types_head_t<                middle, TsTempl<Ts...>>>,
                sort_t<types_tail_t<sizeof...(Ts) - middle, TsTempl<Ts...>>>
            >;
        };

    public:
        using type = sort_t<TsTempl<Types...>>;
    };



    // quick sort

    template <template <typename...> typename TsTempl, typename... Types>
    struct types_quick_sort<TsTempl<Types...>>
    {
    private:
        struct PASS {};

        template <typename...   > struct collapse;
        template <typename... Ts> using  collapse_t = typename collapse<Ts...>::type;

        template <>
        struct collapse<>
        {
            using type = TsTempl<>;
        };

        template <typename F, typename... Ts>
        struct collapse<F, Ts...>
        {
            using type = std::conditional_t<std::is_same_v<F, PASS>, collapse_t<Ts...>, types_summ_t<F, collapse_t<Ts...>>>;
        };

        template <typename Cond, typename... Ts>
        using left = collapse_t< std::conditional_t<type_less_v <Ts, Cond>, Ts, PASS> ... >;

        template <typename Cond, typename... Ts>
        using right = collapse_t< std::conditional_t<type_less_v <Ts, Cond>, PASS, Ts> ... >;



        template <typename   > struct sort;
        template <typename TS> using  sort_t = typename sort<TS>::type;

        template <>
        struct sort<TsTempl<>>
        {
            using type = TsTempl<>;
        };

        template <typename T>
        struct sort<TsTempl<T>>
        {
            using type = TsTempl<T>;
        };

        template <typename A, typename B>
        struct sort<TsTempl<A, B>>
        {
            using type = std::conditional_t<type_less_v<A, B>, TsTempl<A, B>, TsTempl<B, A>>;
        };

        template <typename...> struct Ls_M_Rs;

        template <typename M, typename... Ls, typename... Rs>
        struct Ls_M_Rs<TsTempl<Ls...>, M, TsTempl<Rs...>>
        {
            using res = TsTempl<Ls..., M, Rs...>;
        };

        template <typename F, typename... Ts>
        struct sort<TsTempl<F, Ts...>>
        {
            using type = typename Ls_M_Rs<
                sort_t<left <F, Ts...>>,
                F,
                sort_t<right<F, Ts...>>
            >::res;
        };

    public:
        using type = sort_t<TsTempl<Types...>>;
    };

} // namespace sib
