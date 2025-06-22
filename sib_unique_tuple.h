#pragma once

#include "sib_wrapper.h"

namespace sib {

    // Сортировка типов ----------------------------------------------------------------

    /* https://stackoverflow.com/a/64795244/23601704 */

    template <typename... Ts> struct TTypeList;

    template <typename...> struct TConcatTypeList;

    template <typename... Ts, typename... Us>
    struct TConcatTypeList<TTypeList<Ts...>, TTypeList<Us...>>
    {
        using type = TTypeList<Ts..., Us...>;
    };

    template <typename... Ts>
    using TConcatTypeList_t = typename TConcatTypeList<Ts...>::type;

    template <typename Left, typename Right>
    static constexpr bool type_less = sib::type_name<Left>() < sib::type_name<Right>();

    template <template <typename...> typename Template, typename... Types>
    class TSortTypeListAndInstantiate
    {
    private:
        template <template <typename...> typename Templ, typename...> struct instantiate;

        template <template <typename...> typename Templ, typename... Ts>
        struct instantiate<Templ, TTypeList<Ts...>>
        {
            using type = Templ<Ts...>;
        };

        template <template <typename...> typename Templ, typename... Ts>
        using instantiate_t = typename instantiate<Templ, Ts...>::type;

        template <int Count, typename... Ts>
        struct take;

        template <int Count, typename... Ts>
        using take_t = typename take<Count, Ts...>::type;

        template <typename... Ts>
        struct take<0, TTypeList<Ts...>>
        {
            using type = TTypeList<>;
            using rest = TTypeList<Ts...>;
        };

        template <typename A, typename... Ts>
        struct take<1, TTypeList<A, Ts...>>
        {
            using type = TTypeList<A>;
            using rest = TTypeList<Ts...>;
        };

        template <int Count, typename A, typename... Ts>
        struct take<Count, TTypeList<A, Ts...>>
        {
            using type = TConcatTypeList_t<TTypeList<A>, take_t<Count - 1, TTypeList<Ts...>>>;
            using rest = typename take<Count - 1, TTypeList<Ts...>>::rest;
        };

        template <typename...> struct sort_list;

        template <typename... Ts>
        using sorted_list_t = typename sort_list<Ts...>::type;

        template <typename A>
        struct sort_list<TTypeList<A>>
        {
            using type = TTypeList<A>;
        };

        template <typename A, typename B>
        struct sort_list<TTypeList<A, B>>
        {
            using type = std::conditional_t<type_less<A, B>, TTypeList<A, B>, TTypeList<B, A>>;
        };

        template <typename...> struct merge;

        template <typename... Ts>
        using merge_t = typename merge<Ts...>::type;

        template <typename... Bs>
        struct merge<TTypeList<>, TTypeList<Bs...>>
        {
            using type = TTypeList<Bs...>;
        };

        template <typename... As>
        struct merge<TTypeList<As...>, TTypeList<>>
        {
            using type = TTypeList<As...>;
        };

        template <typename AHead, typename... As, typename BHead, typename... Bs>
        struct merge<TTypeList<AHead, As...>, TTypeList<BHead, Bs...>>
        {
            using type = std::conditional_t<
                type_less<AHead, BHead>,
                TConcatTypeList_t<TTypeList<AHead>, merge_t<TTypeList<As...>, TTypeList<BHead, Bs...>>>,
                TConcatTypeList_t<TTypeList<BHead>, merge_t<TTypeList<AHead, As...>, TTypeList<Bs...>>>
            >;
        };

        template <typename... Ts>
        struct sort_list<TTypeList<Ts...>>
        {
            static constexpr auto first_size = sizeof...(Ts) / 2;
            using split = take<first_size, TTypeList<Ts...>>;
            using type = merge_t<
                sorted_list_t<typename split::type>,
                sorted_list_t<typename split::rest>
            >;
        };

    public:
        using result_type = instantiate_t<Template, sorted_list_t<TTypeList<Types...>>>;
    };

    template <typename T, typename... Ts>
    constexpr bool is_sorted_v = type_less<T, First_t<Ts...>> and is_sorted_v<Ts...>;

    template <typename T>
    constexpr bool is_sorted_v<T> = true;



    // Кортеж уникальных отсортированных типов -----------------------------------------
    template <typename... Ts>
    class TUniqueTuple : public TWrapper<Ts> ...
    {
        static_assert
            (
                sib::is_unique_v<Ts...>,
                "sib::unique_tuple: Template types must be unique and not not be implicitly convertible."
                );

        static_assert
            (
                sib::is_sorted_v<Ts...>,
                "sib::unique_tuple: Template types must be sorted."
                );
    private:
        template <ConvertibleFromTo<Ts...> AnyT>
        using conversion_base_type_from = select_conversion_from_to_t<AnyT, Ts...>;

        template <ConvertibleFromTo<Ts...> AnyT>
        using conversion_type_from = TWrapper<conversion_base_type_from<AnyT>>;

        template <ConvertibleToFrom<Ts...> AnyT>
        using conversion_base_type_to = select_conversion_to_from_t<AnyT, Ts...>;

        template <ConvertibleToFrom<Ts...> AnyT>
        using conversion_type_to = TWrapper<conversion_base_type_to<AnyT>>;
    public:
        template <ConvertibleFromTo<Ts...>... Args>
        TUniqueTuple(Args&&... args) noexcept : conversion_type_from<Args>(args) ... {}

        template <ConvertibleFromTo<Ts...> AnyT>
        constexpr auto operator= (AnyT&& other)
            noexcept(noexcept( conversion_type_from<AnyT>::operator=(std::declval<AnyT>()) ))
        { return conversion_type_from<AnyT>::operator=(std::forward<AnyT>(other)); }

        template <AnyOf<Ts...> T> constexpr T const & as() const noexcept { return *this; };
        template <AnyOf<Ts...> T> constexpr T       & as()       noexcept { return *this; };

        //template <ConvertibleToFrom<Ts...> AnyT> constexpr operator AnyT const & () const noexcept { return *this; }
        //template <ConvertibleToFrom<Ts...> AnyT> constexpr operator AnyT       & ()       noexcept { return TWrapper<select_conversion_t<, TAny>>; }
    };

    template <typename... Ts>
    using MakeUniqueTuple = typename TSortTypeListAndInstantiate<TUniqueTuple, Ts...>::result_type;

    //template <typename... Ts>
    //struct MakeUniqueTuple_Spec
    //{
    //    using type = typename TSortTypeListAndInstantiate<TUniqueTuple, Ts...>::result_type;
    //};

    //template <typename T>
    //struct MakeUniqueTuple_Spec<T>
    //{
    //    using type = TWrapper<T>;
    //};

    //template <typename... Ts>
    //using MakeUniqueTuple = typename MakeUniqueTuple_Spec<Ts...>::type;

} // namespace sib
