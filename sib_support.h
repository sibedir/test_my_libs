#pragma once

#include <iostream>
#include <concepts>
#include <string>
#include <set>
#include <map>
#include <functional>
#include <utility>

namespace sib {

    template <typename...>
    using always_false = std::false_type;

    template <typename... Ts>
    constexpr bool always_false_v = always_false<Ts...>::value;

    template <typename...>
    using always_void_t = void;



    // ----------------------------------------------------------------------------------- type traits

    // instantiation of template

    template <typename T, template <typename...> typename Templ>
    constexpr bool is_instantiation_of_v = false;

    template <template <typename...> typename Templ, typename... Ts>
    constexpr bool is_instantiation_of_v<Templ<Ts...>, Templ> = true;

    template <typename T, template <typename...> typename Templ>
    constexpr bool not_instantiation_of_v = not is_instantiation_of_v<T, Templ>;

    template <typename T, template <typename...> typename Templ> concept InstantiationOf = is_instantiation_of_v<T, Templ>;



    template <template <typename...> typename Templ, typename... Ts>
    struct instantiate_templ
    {
        using type = Templ<Ts...>;
    };

    template <template <typename...> typename Templ, typename... Ts>
    using instantiate_templ_t = typename instantiate_templ<Templ, Ts...>::type;



    template <template <typename...> typename Templ, typename Like>
    struct instantiate_like;

    template <template <typename...> typename Templ, template <typename...> typename Like, typename... Ts>
    struct instantiate_like<Templ, Like<Ts...>>
    {
        using type = Templ<Ts...>;
    };

    template <template <typename...> typename Templ, typename Like>
    using instantiate_like_t = typename instantiate_like<Templ, Like>::type;



    // ----------------------------------------------------------------------------------- type name

    /* https://stackoverflow.com/a/64490578/23601704 */

    namespace detail {

        template <typename T>
        constexpr std::string_view __STN__() noexcept
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
        constexpr auto prefix = detail::__STN__<void>().find("void");
        constexpr auto suffix = detail::__STN__<void>().size() - prefix - 4;

        auto name = detail::__STN__<T>();

        name.remove_prefix(prefix);
        name.remove_suffix(suffix);

        return name;
    }


    template <typename T>
    constexpr auto static_type_name(T&& arg) noexcept
    {
        return static_type_name<decltype(std::forward<T>(arg))>();
    }



    // ----------------------------------------------------------------------------------- forward_like

    template<class T, class U>
    constexpr auto&& forward_like(U&& x) noexcept
    {
        constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
        if constexpr (std::is_lvalue_reference_v<T&&>)
        {
            if constexpr (is_adding_const)
                return std::as_const(x);
            else
                return static_cast<U&>(x);
        }
        else
        {
            if constexpr (is_adding_const)
                return std::move(std::as_const(x));
            else
                return std::move(x);
        }
    }


    // ----------------------------------------------------------------------------------- containers of types

    // INTERFACE

    template<int      N> struct int_tag;
    template<typename T> struct type_tag;

    struct container_of_types {}; // base type for containers of types

    template <typename...> struct type_pack;
    template <typename...> struct type_list;

    template <typename...> struct types_info_helper;
    template <typename...> struct types_info_spec;

    template <typename...> struct types_first;
    template <typename...> struct types_last;

    template <typename, typename> struct types_summ;

    template <size_t, typename> struct types_head;
    template <size_t, typename> struct types_tail;

    template <typename...> struct types_sorted;
    template <typename...> struct types_sequence;

    // USING & CONST

    template <typename L, typename R> constexpr bool type_less_v = (sib::static_type_name<L>() < sib::static_type_name<R>());
    template <typename L, typename R> constexpr bool type_more_v = (sib::static_type_name<L>() > sib::static_type_name<R>());
    template <typename L, typename R> constexpr bool type_equal_v = std::is_same_v<L, R>;

    template <typename... Ts> using types_info = typename types_info_spec<Ts...>::type;

    template <typename... Ts> using types_first_t = typename types_first<Ts...>::type;
    template <typename... Ts> using types_last_t = typename types_last <Ts...>::type;

    template <typename Ts1, typename Ts2> using types_summ_t = typename types_summ<Ts1, Ts2>::type;

    template <size_t N, typename Ts> using types_head_t = typename types_head<N, Ts>::type;
    template <size_t N, typename Ts> using types_tail_t = typename types_tail<N, Ts>::type;

    template <typename... Ts> using types_sorted_t = typename types_sorted   <Ts...>::types;
    template <typename... Ts> using types_sequence_t = typename types_sequence <Ts...>::types;

    // IMPLEMENTATION

    // tags
    template<int N> struct int_tag { static constexpr int value = N; };
    template<typename T> struct type_tag { using type = T; };



    // instantiation
    template <
        template <typename...> typename Templ,
        template <typename...> typename TsTempl,
        typename... Ts
    >
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    struct instantiate_templ<Templ, TsTempl<Ts...>>
    {
        using type = Templ<Ts...>;
    };



    // type_pack

    template <typename... Ts> struct type_pack : container_of_types {};

    template <typename T>
    using get_types_pack = instantiate_like_t<type_pack, T>;


    // type_list

    template <>
    struct type_list<> : container_of_types
    {
        template <size_t N> requires(N == 0)
            static consteval auto get_tail()
        {
            return type_list<>{};
        }
    };

    template <typename F, typename... Ts>
    struct type_list<F, Ts...> : type_list<Ts...>
    {
        template <size_t N, typename... Ps>
        consteval auto get_head()
        {
            if constexpr (N == 0) { return type_list<Ps...>{}; }
            else { return type_list<Ts...>{}.get_head<N - 1, Ps..., F>(); }
        }

        template <size_t N>
        consteval auto get_tail()
        {
            if constexpr (N == 0) { return type_list<F, Ts...>{}; }
            else { return type_list<   Ts...>{}.get_tail<N - 1>(); }
        }
    };

    template <typename T>
    using get_types_list = instantiate_like_t<type_list, T>;



    // info spec

    template <typename... Ts>
    struct types_info_spec {
        using type = types_info_helper<Ts...>;
    };

    template <template <typename...> typename TsTempl, typename... Ts>
    struct types_info_spec<TsTempl<Ts...>>
    {
        using type = types_info_helper<Ts...>;
    };



    // info helper

    template <typename... Ts>
    struct types_info_helper
    {
        template <template <typename...> typename TsTempl = type_pack>
        //requires(std::is_base_of_v<container_of_types, TsTempl<>>)
        using types = TsTempl<Ts...>;

        static constexpr size_t count = sizeof...(Ts);
    };



    // first

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



    // last

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



    // summ

    template <template <typename...> typename Templ, typename... Ts1, typename... Ts2>
        requires(std::is_base_of_v<container_of_types, Templ<>>)
    struct types_summ<Templ<Ts1...>, Templ<Ts2...>>
    {
        using type = Templ<Ts1..., Ts2...>;
    };

    template <typename T, template <typename...> typename Templ, typename... Ts>
        requires(std::is_base_of_v<container_of_types, Templ<>>
    and not_instantiation_of_v<T, Templ>)
        struct types_summ<T, Templ<Ts...>>
    {
        using type = Templ<T, Ts...>;
    };

    template <typename T, template <typename...> typename Templ, typename... Ts>
        requires(std::is_base_of_v<container_of_types, Templ<>>
    and not_instantiation_of_v<T, Templ>)
        struct types_summ<Templ<Ts...>, T>
    {
        using type = Templ<Ts..., T>;
    };



    // head

//#define GPTHEAD
#ifdef GPTHEAD

#include <tuple>
#include <type_traits>

// Вспомогательная структура для получения N первых типов из tuple
    template <std::size_t N, typename Tuple>
    struct tuple_head_impl;

    template <std::size_t N, typename... Ts>
    struct tuple_head_impl<N, std::tuple<Ts...>> {
        static_assert(N <= sizeof...(Ts), "N is larger than number of types");

        // Индексный срез
        template <std::size_t... Is>
        static auto head(std::index_sequence<Is...>) -> std::tuple<std::tuple_element_t<Is, std::tuple<Ts...>>...>;

        using type = decltype(head(std::make_index_sequence<N>{}));
    };

    // Преобразование tuple в шаблон произвольного типа
    template <typename Tuple, template <typename...> typename Templ>
    struct tuple_to_template;

    template <template <typename...> typename Templ, typename... Ts>
    struct tuple_to_template<std::tuple<Ts...>, Templ> {
        using type = Templ<Ts...>;
    };

    // Основное решение
    template <std::size_t N, template <typename...> typename Templ, typename... Ts>
    struct types_head<N, Templ<Ts...>> {
        using tuple_type = std::tuple<Ts...>;
        using head_tuple = typename tuple_head_impl<N, tuple_type>::type;
        using type = typename tuple_to_template<head_tuple, Templ>::type;
    };

#else

    template <size_t N, template <typename...> typename Templ, typename... Ts>
    struct types_head<N, Templ<Ts...>>
    {
    private:
        template <size_t I, typename... L, typename M, typename... R>
        static consteval auto impl(Templ<L...>, Templ<M, R...>)
        {
            if constexpr (I == 0) return std::declval<Templ<L...>>(); else
                if constexpr (I == 1) return std::declval<Templ<L..., M>>(); else
                    return impl<I - 1>(std::declval<Templ<L..., M>>(), std::declval<Templ<R...>>());
        }
    public:
        using type = decltype(impl<N>(std::declval<Templ<>>(), std::declval<Templ<Ts...>>()));
    };

#endif // !MYHEAD


    // tail

    template <size_t N, template <typename...> typename Templ, typename... Ts>
        requires (N == sizeof...(Ts))
    struct types_tail<N, Templ<Ts...>>
    {
        using type = Templ<Ts...>;
    };

    template <size_t N, template <typename...> typename Templ, typename First, typename... Rest>
        requires (N < (sizeof...(Rest) + 1))
    struct types_tail<N, Templ<First, Rest...>>
    {
        using type = typename types_tail<N, Templ<Rest...>>::type;
    };



    // sort

    template <typename... Ts>
    struct types_sorted<Ts...> : types_sorted<type_pack<Ts...>> {};

    // https://stackoverflow.com/a/64795244/23601704
    template <template <typename...> typename TsTempl, typename... Types>
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    struct types_sorted<TsTempl<Types...>>
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
                types_summ_t<TsTempl<A>, merge_t<TsTempl<As...>, TsTempl<B, Bs...>>>,
                types_summ_t<TsTempl<B>, merge_t<TsTempl<A, As...>, TsTempl<Bs...>>>
            >;
        };



        template <typename   > struct sort;
        template <typename TL> using  sort_t = typename sort<TL>::pack;

        template <>
        struct sort<TsTempl<>>
        {
            using pack = TsTempl<>;
        };

        template <typename T>
        struct sort<TsTempl<T>>
        {
            using pack = TsTempl<T>;
        };

        template <typename A, typename B>
        struct sort<TsTempl<A, B>>
        {
            using pack = std::conditional_t<type_less_v<A, B>, TsTempl<A, B>, TsTempl<B, A>>;
        };

        template <typename... Ts>
        struct sort<TsTempl<Ts...>>
        {
            static constexpr size_t middle = sizeof...(Ts) / 2;
            using pack = merge_t<
                sort_t<types_head_t<                middle, TsTempl<Ts...>>>,
                sort_t<types_tail_t<sizeof...(Ts) - middle, TsTempl<Ts...>>>
            >;
        };

    public:
        using types = sort_t<TsTempl<Types...>>;
    };



    // sequence

    template <typename... Ts>
    struct types_sequence<Ts...> : types_sequence<type_pack<Ts...>> {};

    template <template <typename...> typename TsTempl, typename... Types>
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    struct types_sequence<TsTempl<Types...>>
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
        using left = collapse_t< std::conditional_t<type_less_v<Ts, Cond>, Ts, PASS> ... >;

        template <typename Cond, typename... Ts>
        using right = collapse_t< std::conditional_t<type_more_v<Ts, Cond>, Ts, PASS> ... >;

        template <typename   > struct organizer;
        template <typename Ts> using  organizer_t = typename organizer<Ts>::types;

        template <>
        struct organizer<TsTempl<>>
        {
            using types = TsTempl<>;
        };

        template <typename T>
        struct organizer<TsTempl<T>>
        {
            using types = TsTempl<T>;
        };

        template <typename A, typename B>
        struct organizer<TsTempl<A, B>>
        {
            using types = std::conditional_t<type_less_v<A, B>, TsTempl<A, B>, TsTempl<B, A>>;
        };

        template <typename...> struct Ls_M_Rs;

        template <typename M, typename... Ls, typename... Rs>
        struct Ls_M_Rs<TsTempl<Ls...>, M, TsTempl<Rs...>>
        {
            using res = TsTempl<Ls..., M, Rs...>;
        };

        template <typename F, typename... Ts>
        struct organizer<TsTempl<F, Ts...>>
        {
            using types = typename Ls_M_Rs<
                typename organizer_t<left <F, Ts...>>,
                F,
                typename organizer_t<right<F, Ts...>>
            >::res;
        };

    public:
        using types = organizer_t<TsTempl<Types...>>;
    };



    // ----------------------------------------------------------------------------------- type traits

    // any of ...

    template <typename T, typename... Ts> constexpr bool  is_any_of_v = (std::is_same_v<T, Ts> or ...);
    template <typename T, typename... Ts> constexpr bool not_any_of_v = not is_any_of_v<T, Ts...>;

    template <typename T, typename... Ts> concept    AnyOf = is_any_of_v<T, Ts...>;
    template <typename T, typename... Ts> concept NotAnyOf = not_any_of_v<T, Ts...>;



    // one of ...

    //template <typename, typename...> struct is_one_of;

    template <typename...> struct is_one_of : std::false_type {};

    template <typename T, typename First, typename... Rest>
    struct is_one_of<T, First, Rest...> : std::bool_constant<
            (    std::is_same_v<T, First> and not_any_of_v<T, Rest...>)
         or (not std::is_same_v<T, First> and  is_one_of  <T, Rest...>::value)
    >{};

    template <typename T, typename... Ts> constexpr bool  is_one_of_v =     is_one_of<T, Ts...>::value;
    template <typename T, typename... Ts> constexpr bool not_one_of_v = not is_one_of<T, Ts...>::value;

    template <typename T, typename... Ts> concept    OneOf =  is_one_of_v<T, Ts...>;
    template <typename T, typename... Ts> concept NotOneOf = not_one_of_v<T, Ts...>;



    // convert From To...

    template <typename From, typename To>
    constexpr bool is_convertible_from_to_v = std::is_convertible_v<From, To>;

    template <typename From, typename To>
    concept ConvertibleFromTo = is_convertible_from_to_v<From, To>;

    template <typename From, typename... To>
    constexpr bool is_convertible_from_toanyof_v = (is_convertible_from_to_v<From, To> or ...);

    template <typename From, typename... To>
    concept ConvertibleFromToAnyOf = is_convertible_from_toanyof_v<From, To...>;



    template <typename From, typename To, typename... To_other> struct convert_from_tooneof;

    template <typename From, typename To, typename... To_other>
    constexpr bool is_convertible_from_tooneof_v = convert_from_tooneof<From, To, To_other...>::exist;

    template <typename From, typename... To>
    using convert_from_tooneof_t = typename convert_from_tooneof<From, To...>::type;

            template <typename From, typename To>
            struct convert_from_tooneof<From, To>
            {
                static constexpr bool exist = false;
            };

            template <typename From, typename To>
                requires (is_convertible_from_to_v<From, To>)
            struct convert_from_tooneof<From, To>
            {
                static constexpr bool exist = true;
                using type = To;
            };
          
            template <typename From, typename To, typename... To_other>
            struct convert_from_tooneof<From, To, To_other...>
            {
                static constexpr bool exist = false;
            };
          
            template <typename From, typename To, typename... To_other>
                requires (     (    is_convertible_from_to_v     <From, To>         )
                           and (not is_convertible_from_toanyof_v<From, To_other...>) )
            struct convert_from_tooneof<From, To, To_other...>
            {
                static constexpr bool exist = true;
                using type = To;
            };
          
            template <typename From, typename To, typename... To_other>
                requires (     (not is_convertible_from_to_v     <From, To>         )
                           and (    is_convertible_from_tooneof_v<From, To_other...>) )
            struct convert_from_tooneof<From, To, To_other...>
            {
                static constexpr bool exist = true;
                using type = typename convert_from_tooneof<From, To_other...>::type;
            };

    template <typename From, typename... To>
    concept ConvertibleFromToOneOf = is_convertible_from_tooneof_v<From, To...>;



    // convert To From...

    template <typename To, typename From>
    constexpr bool  is_convertible_to_from_v =     std::is_convertible_v<From, To>;

    template <typename To, typename From>
    constexpr bool not_convertible_to_from_v = not std::is_convertible_v<From, To>;

    template <typename To, typename From>
    concept ConvertibleToFrom = is_convertible_to_from_v<To, From>;

    template <typename To, typename... From>
    constexpr bool  is_convertible_to_fromanyof_v = ( is_convertible_to_from_v<To, From> or  ...);

    template <typename To, typename... From>
    constexpr bool not_convertible_to_fromanyof_v = (not_convertible_to_from_v<To, From> and ...);

    template <typename To, typename... From>
    concept ConvertibleToFromAnyOf = is_convertible_to_fromanyof_v<To, From...>;



    template <typename To, typename From, typename... From_other> struct convert_to_fromoneof;

    template <typename To, typename From, typename... From_other>
    constexpr bool  is_convertible_to_fromoneof_v =     convert_to_fromoneof<To, From, From_other...>::exist;

    template <typename To, typename From, typename... From_other>
    constexpr bool not_convertible_to_fromoneof_v = not convert_to_fromoneof<To, From, From_other...>::exist;

    template <typename To, typename... From>
    using convert_to_fromoneof_t = typename convert_to_fromoneof<To, From...>::type;

            template <typename To, typename... From>
            struct convert_to_fromoneof<To, From...>
            {
                static constexpr bool exist = false;
            };

            template <typename To, typename From>
                requires (is_convertible_to_from_v<To, From>)
            struct convert_to_fromoneof<To, From>
            {
                static constexpr bool exist = true;
                using type = From;
            };
          
            template <typename To, typename From, typename... From_other>
                requires (     (    is_convertible_to_from_v     <To, From>         )
                           and (not is_convertible_to_fromanyof_v<To, From_other...>) )
            struct convert_to_fromoneof<To, From, From_other...>
            {
                static constexpr bool exist = true;
                using type = From;
            };
          
            template <typename To, typename From, typename... From_other>
                requires (     (not is_convertible_to_from_v     <To, From>         )
                           and (    is_convertible_to_fromoneof_v<To, From_other...>) )
            struct convert_to_fromoneof<To, From, From_other...>
            {
                static constexpr bool exist = true;
                using type = typename convert_to_fromoneof<To, From_other...>::type;
            };

    template <typename To, typename... From>
    concept ConvertibleToFromOneOf = is_convertible_to_fromoneof_v<To, From...>;



    // assignment operator from Source to Dest...

    template <typename Dest, typename Source = Dest>
    concept HasAssignmentOperator = requires(Dest& d, Source&& s) {
        { d = std::forward<Source>(s) };
    };

    template <typename Dest, typename Source = Dest>
    constexpr bool has_assignment_operator_v = HasAssignmentOperator<Dest, Source>;

    template <typename Dest, typename Source = Dest>
    struct has_assignment_operator : std::bool_constant<has_assignment_operator_v<Dest, Source>> {};

    template <typename Dest, typename Source = Dest> struct assignment_operator_result;
    
    template <typename Source, HasAssignmentOperator<Source> Dest>
    struct assignment_operator_result<Dest, Source>
    {
        using type = decltype(std::declval<Dest&>() = std::declval<Source>());
    };

    template <typename Dest, typename Source = Dest>
    using assignment_operator_result_t = typename assignment_operator_result<Dest, Source>::type;



    template <typename Source, typename... Dest> struct assignment_operator_from_tooneof;

    template <typename Source, typename... Dest>
    constexpr bool has_assignment_operator_from_tooneof_v = assignment_operator_from_tooneof<Source, Dest...>::has;

    template <typename Source, typename... Dest>
    using assignment_operator_from_tooneof_select = typename assignment_operator_from_tooneof<Source, Dest...>::select;

    template <typename Source, typename... Dest>
    using assignment_operator_from_tooneof_result = typename assignment_operator_from_tooneof<Source, Dest...>::result;

        template <typename Source, typename... Dest>
        struct assignment_operator_from_tooneof<Source, Dest...>
        {
            static constexpr bool has = false;
        };

        template <typename Source, typename Dest>
            requires (has_assignment_operator_v<Dest, Source>)
        struct assignment_operator_from_tooneof<Source, Dest>
        {
            static constexpr bool has = true;
            using select = Dest;
            using result = assignment_operator_result_t<Dest, Source>;
        };

        template <typename Source, typename Dest, typename... Dest_other>
        struct assignment_operator_from_tooneof<Source, Dest, Dest_other...>
        {
            static constexpr bool has = false;
        };

        template <typename Source, typename Dest, typename... Dest_other>
                requires (     (    has_assignment_operator_v             <Dest  , Source       >)
                           and (not has_assignment_operator_from_tooneof_v<Source, Dest_other...>) )
        struct assignment_operator_from_tooneof<Source, Dest, Dest_other...>
        {
            static constexpr bool has = true;
            using select = Dest;
            using result = assignment_operator_result_t<Dest, Source>;
        };

        template <typename Source, typename Dest, typename... Dest_other>
                requires (     (not has_assignment_operator_v             <Dest  , Source       >)
                           and (    has_assignment_operator_from_tooneof_v<Source, Dest_other...>) )
        struct assignment_operator_from_tooneof<Source, Dest, Dest_other...>
        {
            static constexpr bool has = true;
            using select = typename assignment_operator_from_tooneof<Source, Dest_other...>::select;
            using result = typename assignment_operator_from_tooneof<Source, Dest_other...>::result;
        };

    template <typename Source, typename... Dest>
    concept HasAssignmentOperatorFromToOneOf = has_assignment_operator_from_tooneof_v<Source, Dest...>;



    // construct From To...

    template <typename From, typename To>
    constexpr bool is_constructible_from_to_v = std::is_constructible_v<To, From>;

    template <typename From, typename To>
    concept ConstructibleFromTo = is_constructible_from_to_v<From, To>;

    template <typename From, typename... To>
    constexpr bool is_constructible_from_toanyof_v = (is_constructible_from_to_v<From, To> or ...);

    template <typename From, typename... To>
    concept ConstructibleFromToAnyOf = is_constructible_from_toanyof_v<From, To...>;



    template <typename From, typename To, typename... To_other> struct construct_from_tooneof;

    template <typename From, typename To, typename... To_other>
    constexpr bool is_constructible_from_tooneof_v = construct_from_tooneof<From, To, To_other...>::exist;

    template <typename From, typename... To>
    using construct_from_tooneof_t = typename construct_from_tooneof<From, To...>::type;

            template <typename From, typename To>
            struct construct_from_tooneof<From, To>
            {
                static constexpr bool exist = false;
            };

            template <typename From, typename To>
                requires (is_constructible_from_to_v<From, To>)
            struct construct_from_tooneof<From, To>
            {
                static constexpr bool exist = true;
                using type = To;
            };
          
            template <typename From, typename To, typename... To_other>
            struct construct_from_tooneof<From, To, To_other...>
            {
                static constexpr bool exist = false;
            };
          
            template <typename From, typename To, typename... To_other>
                requires (     (    is_constructible_from_to_v     <From, To>         )
                           and (not is_constructible_from_toanyof_v<From, To_other...>) )
            struct construct_from_tooneof<From, To, To_other...>
            {
                static constexpr bool exist = true;
                using type = To;
            };
          
            template <typename From, typename To, typename... To_other>
                requires (     (not is_constructible_from_to_v     <From, To>         )
                           and (    is_constructible_from_tooneof_v<From, To_other...>) )
            struct construct_from_tooneof<From, To, To_other...>
            {
                static constexpr bool exist = true;
                using type = typename construct_from_tooneof<From, To_other...>::type;
            };

    template <typename From, typename... To>
    concept ConstructibleFromToOneOf = is_constructible_from_tooneof_v<From, To...>;



    // construct To From...

    template <typename To, typename From>
    constexpr bool is_constructible_to_from_v = std::is_constructible_v<To, From>;

    template <typename To, typename From>
    concept ConstructibleToFrom = is_constructible_to_from_v<To, From>;

    template <typename To, typename... From>
    constexpr bool  is_constructible_to_fromanyof_v = (is_constructible_to_from_v<To, From> or ...);

    template <typename To, typename... From>
    concept ConstructibleToFromAnyOf = is_constructible_to_fromanyof_v<To, From...>;



    template <typename To, typename From, typename... From_other> struct construct_to_fromoneof;

    template <typename To, typename From, typename... From_other>
    constexpr bool is_constructible_to_fromoneof_v = construct_to_fromoneof<To, From, From_other...>::exist;

    template <typename To, typename... From>
    using construct_to_fromoneof_t = typename construct_to_fromoneof<To, From...>::type;

            template <typename To, typename From>
            struct construct_to_fromoneof<To, From>
            {
                static constexpr bool exist = false;
            };

            template <typename To, typename From>
                requires (is_constructible_to_from_v<To, From>)
            struct construct_to_fromoneof<To, From>
            {
                static constexpr bool exist = true;
                using type = From;
            };
          
            template <typename To, typename From, typename... From_other>
            struct construct_to_fromoneof<To, From, From_other...>
            {
                static constexpr bool exist = false;
            };
          
            template <typename To, typename From, typename... From_other>
                requires (     (    is_constructible_to_from_v     <To, From>         )
                           and (not is_constructible_to_fromanyof_v<To, From_other...>) )
            struct construct_to_fromoneof<To, From, From_other...>
            {
                static constexpr bool exist = true;
                using type = From;
            };
          
            template <typename To, typename From, typename... From_other>
                requires (     (not is_constructible_to_from_v     <To, From>         )
                           and (    is_constructible_to_fromoneof_v<To, From_other...>) )
            struct construct_to_fromoneof<To, From, From_other...>
            {
                static constexpr bool exist = true;
                using type = typename construct_to_fromoneof<To, From_other...>::type;
            };

    template <typename To, typename... From>
    concept ConstructibleToFromOneOf = is_constructible_to_fromoneof_v<To, From...>;



    // container

    template <typename T>
    constexpr bool is_container_v =
        // до лучших времён
        // requires ( requires (T v) { for (auto it : v) {} } )
        (
            requires (T & v)
            {
                { std::begin(v) == std::end(v) } -> std::same_as<bool>;
            }
        )
        and
        (
            requires (decltype(std::begin(std::declval<T&>())) it)
            {
                { *it };
                { ++it } -> std::same_as<decltype(it)&>;
            }
        )
    ;

    template <typename T>
    constexpr bool not_container_v = not is_container_v<T>;

    template <typename T> struct  is_container : std::bool_constant< is_container_v<T>> {};
    template <typename T> struct not_container : std::bool_constant<not_container_v<T>> {};

    template <typename T> concept     Container = is_container_v<T>;
    template <typename T> concept not_Container = not_container_v<T>;



    template <Container Cont>
    using container_elem_t = decltype(*std::begin(std::declval<Cont&>()));



    // enum

    template <typename T> constexpr bool  is_enum_v = std::is_enum_v<T>;
    template <typename T> constexpr bool not_enum_v = not is_enum_v<T>;

    template <typename T> struct  is_enum : std::bool_constant< is_enum_v<T>> {};
    template <typename T> struct not_enum : std::bool_constant<not_enum_v<T>> {};

    template <typename T> concept     Enum = is_enum_v<T>;
    template <typename T> concept not_Enum = not_enum_v<T>;



    template <typename T> struct is_enum_class : std::false_type {};

    template <Enum E>
        requires (not std::is_convertible_v<E, std::underlying_type_t<std::remove_cvref_t<E>>>)
    struct is_enum_class<E> : std::true_type {};

    template <typename T> struct not_enum_class : std::bool_constant<not is_enum_class<T>::value> {};

    template <typename T> constexpr bool  is_enum_class_v = is_enum_class<T>::value;
    template <typename T> constexpr bool not_enum_class_v = not_enum_class<T>::value;

    template <typename T> concept     EnumClass = is_enum_class_v<T>;
    template <typename T> concept not_EnumClass = not_enum_class_v<T>;



    // function

    template <typename T> constexpr bool  is_function_v = std::is_function_v<T>;
    template <typename T> constexpr bool not_function_v = not is_function_v<T>;

    template <typename T> struct  is_function : std::bool_constant< is_function_v<T>> {};
    template <typename T> struct not_function : std::bool_constant<not_function_v<T>> {};

    template <typename T> concept     Function = is_function_v<T>;
    template <typename T> concept not_Function = not_function_v<T>;



    template <typename T>
    constexpr bool is_like_function_v = requires (T f) { std::function(f); };

    template <typename T>
    constexpr bool not_like_function_v = not is_like_function_v<T>;

    template <typename T> struct  is_like_function : std::bool_constant< is_like_function_v<T>> {};
    template <typename T> struct not_like_function : std::bool_constant<not_like_function_v<T>> {};

    template <typename T> concept     LikeFunction = is_like_function_v<T>;
    template <typename T> concept not_LikeFunction = not_like_function_v<T>;



    // pointer

    template <typename T>
    constexpr bool is_like_nullptr_v = std::is_constructible_v<std::nullptr_t, T>;

    template <typename T>
    constexpr bool not_like_nullptr_v = not is_like_nullptr_v<T>;

    template <typename T> struct  is_like_nullptr : std::bool_constant< is_like_nullptr_v<T>> {};
    template <typename T> struct not_like_nullptr : std::bool_constant<not_like_nullptr_v<T>> {};

    template <typename T> concept     LikeNullptr = is_like_nullptr_v<T>;
    template <typename T> concept not_LikeNullptr = not_like_nullptr_v<T>;



    template <typename T> constexpr bool  is_pointer_v = std::is_pointer_v<T>;
    template <typename T> constexpr bool not_pointer_v = not is_pointer_v<T>;

    template <typename T> struct  is_pointer : std::bool_constant< is_pointer_v<T>> {};
    template <typename T> struct not_pointer : std::bool_constant<not_pointer_v<T>> {};

    template <typename T> concept     Pointer = is_pointer_v<T>;
    template <typename T> concept not_Pointer = not_pointer_v<T>;



    template <typename T>
    struct is_like_pointer : std::false_type {};

    template <typename T>
        requires (
            std::is_pointer_v<T>
            or
            (       not_function_v<T>
                and
                    std::is_convertible_v<T, void const*>
            )
            or
            (       is_like_pointer<decltype(std::declval<T>().operator->())>::value
                and
                    std::is_same_v<
                        std::remove_pointer_t< decltype(std::declval<T>().operator->()) >,
                        decltype(std::declval<T>().operator *())
                    >
            )
        )
        struct is_like_pointer<T> : std::true_type {};

    template <typename T>
    struct not_like_pointer : std::bool_constant<is_like_pointer<T>::value> {};

    template <typename T> constexpr bool  is_like_pointer_v = is_like_pointer<T>::value;
    template <typename T> constexpr bool not_like_pointer_v = not_like_pointer<T>::value;

    template <typename T> concept     LikePointer = is_like_pointer_v<T>;
    template <typename T> concept not_LikePointer = not_like_pointer_v<T>;



    template <LikePointer P>
        requires (not_like_nullptr_v<P>)
    using base_pointer_type = decltype(std::declval<P>().operator*());



    template <typename T>
    constexpr bool may_be_indirect_v =
            is_like_pointer_v<T>
        and not_function_v<T>
        and requires(T t) { *t; }
    ;

    template <typename T>
    constexpr bool cant_be_indirect_v = not may_be_indirect_v<T>;

    template <typename T> struct  may_be_indirect : std::bool_constant< may_be_indirect_v<T>> {};
    template <typename T> struct cant_be_indirect : std::bool_constant<cant_be_indirect_v<T>> {};

    template <typename T> concept     Indirected = may_be_indirect_v<T>;
    template <typename T> concept not_Indirected = cant_be_indirect_v<T>;



    template <Indirected P>
    using base_indirection_type = std::remove_reference_t<decltype(*std::declval<P>())>;



    // array

    template <typename T> constexpr bool  is_array_v = std::is_array_v<T>;
    template <typename T> constexpr bool not_array_v = not is_array_v<T>;

    template <typename T> using   is_array = std::is_array<T>;
    template <typename T> struct not_array : std::bool_constant<not_array_v<T>> {};

    template <typename T> concept     Array = is_array_v<T>;
    template <typename T> concept not_Array = not_array_v<T>;



    // char

    template <typename T>
    constexpr bool is_char_v = is_any_of_v< std::remove_const_t<T>, char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t >;

    template <typename T>
    constexpr bool not_char_v = not is_char_v<T>;

    template <typename T>
    struct is_char : std::bool_constant<is_char_v<T>> {};

    template <typename T> concept     Char = is_char_v<T>;
    template <typename T> concept not_Char = not_char_v<T>;



    // as_basic_string

    template <typename> struct as_basic_string;

    template <typename T>
    struct is_as_basic_string
    {
        friend as_basic_string<T>;
    private:
        static consteval void _test(...);

        template<Char Ch, typename Tr, typename Al>
        static consteval auto _test(std::basic_string<Ch, Tr, Al> const& arg) { return arg; };
    public:
        static constexpr bool value = !std::is_same_v<decltype(_test(std::declval<T>())), void>;
    };

    template <typename T>
    constexpr bool is_as_basic_string_v = is_as_basic_string<T>::value;

    template <typename T>
    concept IsAsBasicString = is_as_basic_string_v<T>;

    template <IsAsBasicString BS>
    struct as_basic_string<BS>
    {
        using type           = decltype(is_as_basic_string<BS>::_test(std::declval<BS>()));
        using value_type     = typename type::value_type    ; 
        using traits_type    = typename type::traits_type   ;
        using allocator_type = typename type::allocator_type;
    };

    template <IsAsBasicString BS>
    using as_basic_string_t = typename as_basic_string<BS>::type;



    // like_string

    template <typename T>
    struct is_like_string : std::false_type {};

    template <Container T>
        requires (is_char_v<std::remove_cvref_t<container_elem_t<T>>>)
    struct is_like_string<T> : std::true_type {};

    template <typename T>
    struct not_like_string : std::bool_constant<not is_like_string<T>::value> {};

    template <typename T> constexpr bool  is_like_string_v =  is_like_string<T>::value;
    template <typename T> constexpr bool not_like_string_v = not_like_string<T>::value;

    template <typename T> concept     LikeString = is_like_string_v<T>;
    template <typename T> concept not_LikeString = not_like_string_v<T>;



    // unique

    template <typename T, typename... Ts>
    constexpr bool is_unique_v =
            (not is_convertible_to_fromanyof_v<T, Ts...>)
        and (not is_convertible_from_toanyof_v<T, Ts...>)
        and (    is_unique_v<Ts...>)
    ;

    template <typename T>
    constexpr bool is_unique_v<T> = true;

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    constexpr bool is_unique_v<TsTempl<Ts...>> = is_unique_v<Ts...>;

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    constexpr bool is_unique_v<TsTempl<TsTempl<Ts...>>> = true;



    // sorted

    template <typename... Ts>         struct is_sorted;
    template <typename... Ts> constexpr bool is_sorted_v = is_sorted<Ts...>::value;

    //template <typename... Ts>
    //struct is_sorted_helper
    //{
    //    static constexpr size_t middle = sizeof...(Ts) / 2;
    //    using split = types_separat<middle, type_pack<Ts...>>;
    //    using head = typename split::head;
    //    using tail = typename split::tail;

    //    static constexpr bool value =
    //            type_less_v< types_last_t<head>, types_first_t<tail> >
    //        and is_sorted_helper<head>::value
    //        and is_sorted_helper<tail>::value
    //    ;
    //};

    template <>
    struct is_sorted<> : std::bool_constant<true> {};

    template <typename T>
    struct is_sorted<T> : std::bool_constant<true> {};

    template <typename A, typename B, typename... Ts>
    struct is_sorted<A, B, Ts...> : std::bool_constant<type_less_v<A, B> and is_sorted_v<B, Ts...>> {};

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    struct is_sorted<TsTempl<Ts...>> : std::bool_constant<is_sorted_v<Ts...>> {};

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    struct is_sorted<TsTempl<TsTempl<Ts...>>> : std::bool_constant<true> {};



// ----------------------------------------------------------------------------------- pointer

    template <typename T>
    static constexpr inline auto ptr_to_int(T* pointer) noexcept
    {
        return reinterpret_cast<std::uintptr_t const>(pointer);
    }



// ----------------------------------------------------------------------------------- rand

    /* random in [0, 1) */ extern inline double rand();
    /* random in [0, X) */ extern inline double rand(double);



// ----------------------------------------------------------------------------------- extrude

    template<std::integral Int>
    inline Int post_dev(Int& val, Int divisor) noexcept
    {
        Int res = val;
        val /= divisor;
        return res;
    }

    template<std::integral Int>
    inline Int dev_ret_mod(Int& val, Int divisor) noexcept
    {
        return post_dev(val, divisor) % divisor;
    }

    template<std::integral Int>
    inline Int extrude(Int& val, char bits) noexcept
    {
        return dev_ret_mod(val, static_cast<Int>(Int(1) << bits));
    }

    /*
    template<std::integral Int>
    inline Int extrude(Int& val, char bits) noexcept
    {
        Int divisor = (Int(1) << bits);
        Int res = val % divisor;
        val /= divisor;
        return res;
    }

    template<std::integral Int>
    inline Int extrude(Int& val, char bits) noexcept(noexcept(std::div(val, std::declval<Int>())))
    {
        auto res = std::div(val, Int(1) << bits);
        val = res.quot;
        return res.rem;
    }
    */

// ----------------------------------------------------------------------------------- convertion

    extern inline std::wstring string_to_wstring(std::string const& str);

    extern inline std::string wstring_to_string(std::wstring const& str);



// ----------------------------------------------------------------------------------- console std::cout

    using TKeyCode = int;

    template <std::integral Int>
    struct ByteConcat
    {
        unsigned char bytes[sizeof(Int)];
        constexpr Int value() const
        {
            Int res = 0;
            for (int i = sizeof(Int) - 1; i >= 0; --i)
            {
                res = res * 256 + bytes[i];
            }
            return res;
        }
    };

    inline constexpr unsigned char KC_COMM_1 =   0;
    inline constexpr unsigned char KC_COMM_2 = 224;

    inline constexpr TKeyCode KC_ENTER = 13;
    inline constexpr TKeyCode KC_ESC   = 27;

    inline constexpr TKeyCode KC_F1  = (ByteConcat<TKeyCode>{  59, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F2  = (ByteConcat<TKeyCode>{  60, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F3  = (ByteConcat<TKeyCode>{  61, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F4  = (ByteConcat<TKeyCode>{  62, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F5  = (ByteConcat<TKeyCode>{  63, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F6  = (ByteConcat<TKeyCode>{  64, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F7  = (ByteConcat<TKeyCode>{  65, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F8  = (ByteConcat<TKeyCode>{  66, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F9  = (ByteConcat<TKeyCode>{  67, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F10 = (ByteConcat<TKeyCode>{  68, KC_COMM_1 }).value();
    inline constexpr TKeyCode KC_F11 = (ByteConcat<TKeyCode>{ 133, KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_F12 = (ByteConcat<TKeyCode>{ 134, KC_COMM_2 }).value();

    inline constexpr TKeyCode KC_NUM_INSERT    = 82;
    inline constexpr TKeyCode KC_NUM_DELETE    = 83;
    inline constexpr TKeyCode KC_NUM_HOME      = 71;
    inline constexpr TKeyCode KC_NUM_END       = 79;
    inline constexpr TKeyCode KC_NUM_PAGE_UP   = 73;
    inline constexpr TKeyCode KC_NUM_PAGE_DOWN = 81;

    inline constexpr TKeyCode KC_INSERT    = (ByteConcat<TKeyCode>{ KC_NUM_INSERT   , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_DELETE    = (ByteConcat<TKeyCode>{ KC_NUM_DELETE   , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_HOME      = (ByteConcat<TKeyCode>{ KC_NUM_HOME     , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_END       = (ByteConcat<TKeyCode>{ KC_NUM_END      , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_PAGE_UP   = (ByteConcat<TKeyCode>{ KC_NUM_PAGE_UP  , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_PAGE_DOWN = (ByteConcat<TKeyCode>{ KC_NUM_PAGE_DOWN, KC_COMM_2 }).value();

    inline constexpr TKeyCode KC_NUM_LEFT  = 75;
    inline constexpr TKeyCode KC_NUM_RIGHT = 77;
    inline constexpr TKeyCode KC_NUM_UP    = 72;
    inline constexpr TKeyCode KC_NUM_DOWN  = 80;

    inline constexpr TKeyCode KC_LEFT  = (ByteConcat<TKeyCode>{ KC_NUM_LEFT , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_RIGHT = (ByteConcat<TKeyCode>{ KC_NUM_RIGHT, KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_UP    = (ByteConcat<TKeyCode>{ KC_NUM_UP   , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_DOWN  = (ByteConcat<TKeyCode>{ KC_NUM_DOWN , KC_COMM_2 }).value();

    using Tconsole_reactions_to_keys = std::map<TKeyCode, std::function<void()>>;

    extern Tconsole_reactions_to_keys default_console_reactions_to_keys;

    extern inline TKeyCode WaitKeyCodes(
        std::set<TKeyCode> const& codes,
        std::string        const& msg = ""
    );

    extern inline TKeyCode WaitAnyKey(
        std::string const& msg = ""
    );

    extern inline TKeyCode WaitReactToKeyCodes(
        std::set<TKeyCode>         const& codes,
        Tconsole_reactions_to_keys const& console_reactions_to_keys = default_console_reactions_to_keys,
        std::string                const& msg = "");

    extern inline TKeyCode WaitReactToAnyKey(
        Tconsole_reactions_to_keys const& console_reactions_to_keys = default_console_reactions_to_keys,
        std::string                const& msg = ""
    );

} // namespace sib

