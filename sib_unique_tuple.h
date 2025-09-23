#pragma once

#include "sib_type_traits.h"
#include "sib_wrapper.h"
#include <type_traits>

namespace sib {

    template <typename... Ts>
        requires( not (std::is_const_v<Ts> or ...)
                  and is_unique_v<Ts...>
                  and is_sorted_v<Ts...>           )                  
    class TUniqueTuple :
        public TWrapper<Ts> ...
    {
    private:
        template <typename _T>
        using construction_base_type_from = construct_from_tooneof_select<_T, Ts...>;

        template <typename _T>
        using construction_type_from = TWrapper<construction_base_type_from<_T>>;

        template <typename _T>
        static constexpr bool noexcept_constr = noexcept(TWrapper<construct_from_tooneof_select<_T, Ts...>>(std::declval<_T>()));

        template <typename... _Ts>
        static constexpr bool noexcept_constr_ts = (noexcept_constr<_Ts> and ...);
    public:
        template <template <typename...> typename TsTempl = types_pack>
            requires(std::is_base_of_v<types_container, TsTempl<>>)
        using types = TsTempl<Ts...>;

        template <template <typename...> typename TsTempl = types_pack>
            requires(std::is_base_of_v<types_container, TsTempl<>>)
        using veritable_types = TsTempl<TWrapper<Ts>...>;

        TUniqueTuple() = default;

        template <typename... _Ts>
            requires(     (is_constructible_from_tooneof_v<_Ts, Ts...> and ... )
                      and (no_duplicates_v<construction_base_type_from<_Ts>...>) )
        explicit TUniqueTuple(_Ts&&... other)
            noexcept(noexcept_constr_ts<_Ts...>)
            : construction_type_from<_Ts>(std::forward<_Ts>(other)) ...
        {}

        // template <AnyOf<Ts...> _T>
        // constexpr operator TWrapper<_T>() noexcept { return *this; }

        template <AnyOf<Ts...> _T> constexpr operator _T const & () const noexcept { return *this; }
        template <AnyOf<Ts...> _T> constexpr operator _T       & ()       noexcept { return *this; }

        // get ---------------------------------------------------------------------------------------------------------

        template <typename _T>
            requires( not std::is_rvalue_reference_v<_T>
                      and AnyOf<_T const &, std::remove_reference_t<Ts> const &...> )
        constexpr decltype(auto) get() const
            noexcept(noexcept(static_cast<_T const &>(*this)))
        {
            return static_cast<_T const &>(*this);
        }

        template <typename _T>
            requires( not std::is_rvalue_reference_v<_T>
                      and AnyOf<_T&, std::remove_reference_t<Ts>&...> )
        constexpr decltype(auto) get()
            noexcept(noexcept(static_cast<_T&>(*this)))
        {
            return static_cast<_T&>(*this);
        }

        // as ----------------------------------------------------------------------------------------------------------

        template <typename _T>
            requires(requires(TUniqueTuple ut) { ut.get<_T>(); })
        constexpr decltype(auto) as() const
        {
            return get<_T>();
        }
        
        template <typename _T>
            requires( not requires(TUniqueTuple ut) { ut.get     <_T>(  ); }
                      and requires(TUniqueTuple ut) { static_cast<_T>(ut); } )
        constexpr decltype(auto) as() const
        {
            return static_cast<std::remove_reference_t<_T>>(*this);
        }

        template <typename _T>
            requires(requires(TUniqueTuple ut) { ut.get<_T>(); })
        constexpr decltype(auto) as()
        {
            return get<_T>();
        }

        template <typename _T>
            requires( not requires(TUniqueTuple ut) { ut.get     <_T>(  ); }
                      and requires(TUniqueTuple ut) { static_cast<_T>(ut); } )
        constexpr decltype(auto) as()
        {
            return static_cast<std::remove_reference_t<_T>>(*this);
        }

        // operators ---------------------------------------------------------------------------------------------------

        //template <ConvertibleToFromOneOf<Ts...> AnyT>
        //    requires (not_any_of_v<AnyT, Ts...>)
        //constexpr operator AnyT () const noexcept { return static_cast<AnyT>(static_cast<convert_to_fromoneof_t<AnyT, Ts...>>(*this)); }

        //template <typename AnyT>
        //    requires (not_convertible_to_fromoneof_v<AnyT, Ts...>)
        //constexpr explicit operator AnyT () const noexcept { return as<AnyT>(); }

        template <typename _T>
            requires(is_one_of_v<_T, Ts...>)
        constexpr _T& operator=(_T const & other)
            noexcept(noexcept(std::declval<_T&>() = std::declval<_T>()))
        {
            return get<_T>() = other;
        }

        template <typename _T>
            requires(not is_one_of_v<_T, Ts...>
                     and is_assignable_from_tooneof_v<_T, Ts&...>)
        constexpr assign_from_tooneof_result<_T, Ts&...> operator=(_T const & other)
            noexcept(noexcept(std::declval<assign_from_tooneof_select<_T, Ts&...>>() = std::declval<_T>()))
        {
            return get<assign_from_tooneof_select<_T, Ts&...>>() = other;
        }

        constexpr TUniqueTuple& operator=(TUniqueTuple const & other)
        {
            ((get<Ts>() = other.template get<Ts>()), ...);
            return *this;
        }

        template <typename... _Ts>
            requires(     (is_assignable_from_tooneof_v<_Ts, Ts&...> and ... )
                      and (no_duplicates_v<construction_base_type_from<_Ts>...>) )
        constexpr TUniqueTuple& operator=(TUniqueTuple<_Ts...> const & other)
        {
            ((
                is_assignable_to_fromoneof_v<Ts&, _Ts...>
                ?   (get<Ts>() = other.template get<assign_to_fromoneof_select<Ts&, _Ts...>>())
                :   0
            ), ...);
            return *this;
        }

        template <typename... _Ts>
            requires(     (is_constructible_from_tooneof_v<_Ts, Ts...> and ... )
                      and (no_duplicates_v<construction_base_type_from<_Ts>...>) )
        TUniqueTuple(TUniqueTuple<_Ts...> const & other)
            noexcept(noexcept_constr_ts<_Ts...>)
            : TUniqueTuple(other.template get<_Ts>() ...)
        {}
        
        template <typename _T>
            requires( is_convertible_from_tooneof_v<_T, std::remove_reference_t<Ts>...> )
        auto operator == (_T const & other) const
        {
            return get<convert_from_tooneof_select<_T, std::remove_reference_t<Ts>...>>() == other;
        }
    };

    template <typename... Ts>
        requires(sib::is_unique_v<Ts...>)
    struct MakeUniqueTupleSpec
    {
        using type = instantiate_templ_t<TUniqueTuple, types_quick_sort_t<types_pack<Ts...>>>;
    };

    template <template <typename...> typename TsTempl, typename... Ts>
        requires (std::is_base_of_v<types_container, TsTempl<>>)
    struct MakeUniqueTupleSpec<TsTempl<Ts...>>
    {
        using type = instantiate_templ_t<TUniqueTuple, types_quick_sort_t<TsTempl<Ts...>>>;
    };

    template <typename... Ts> using MakeUniqueTuple = typename MakeUniqueTupleSpec<Ts...>::type;

    template <typename... Args>
    constexpr auto make_unique_tuple(Args&&... args)
        noexcept(noexcept(MakeUniqueTuple<std::remove_reference_t<Args>...>(std::forward<Args>(args)...)))
    {
        return MakeUniqueTuple<std::remove_reference_t<Args>...>(std::forward<Args>(args)...);
    }

} // namespace sib
