#pragma once

#include "sib_support.h"
#include "sib_wrapper.h"

namespace sib {

    template <typename... Ts>
        requires(sib::is_unique_v<Ts...> and sib::is_sorted_v<Ts...>)
    class TUniqueTuple :
        public TWrapper<Ts> ...
    {
    private:
        template <typename AnyT>
        using construction_base_type_from = construct_from_tooneof_t<AnyT, Ts...>;

        template <typename AnyT>
        using construction_type_from = TWrapper<construction_base_type_from<AnyT>>;

        template <typename AnyT>
        static constexpr bool noexcept_constr = noexcept(TWrapper<construct_from_tooneof_t<AnyT, Ts...>>(std::declval<AnyT>()));

        template <typename... AnyTs>
        static constexpr bool noexcept_constr_ts = (noexcept_constr<AnyTs> and ...);
    public:
        template <template <typename...> typename TsTempl = type_pack>
            requires(std::is_base_of_v<container_of_types, TsTempl<>>)
        using types = TsTempl<Ts...>;

        template <template <typename...> typename TsTempl = type_pack>
            requires(std::is_base_of_v<container_of_types, TsTempl<>>)
        using veritable_types = TsTempl<TWrapper<Ts>...>;

        template <ConstructibleFromToOneOf<Ts...>... AnyTs>
            requires( sizeof...(Ts) >= sib::types_info<sib::type_pack<construction_type_from<AnyTs>...>>::count)
        TUniqueTuple(AnyTs&&... other)
            noexcept(noexcept_constr_ts<AnyTs...>)
            : construction_type_from<AnyTs>(std::forward<AnyTs>(other)) ...
        {}

        template <typename AnyT>
        constexpr assignment_operator_from_tooneof_result<AnyT, Ts...> operator= (AnyT&& other)
            noexcept(noexcept(TWrapper<assignment_operator_from_tooneof_select<AnyT, Ts...>>::operator=(std::declval<AnyT>())))
        { return TWrapper<assignment_operator_from_tooneof_select<AnyT, Ts...>>::operator=(std::forward<AnyT>(other)); }

        template <AnyOf<Ts...> AnyT>
        constexpr operator TWrapper<AnyT>() noexcept { return *this; }

        template <AnyOf<Ts...> AnyT> constexpr operator AnyT const & () const noexcept { return *this; }
        template <AnyOf<Ts...> AnyT> constexpr operator AnyT       & ()       noexcept { return *this; }

        template <typename AnyT>
            requires (is_any_of_v<std::remove_reference_t<AnyT> const, std::remove_reference_t<Ts> const...>)
        constexpr decltype(auto) get() const
            noexcept(noexcept(static_cast<std::remove_reference_t<AnyT> const &>(*this)))
        {
            return static_cast<std::remove_reference_t<AnyT> const &>(*this);
        }

        template <typename AnyT>
            requires (is_any_of_v<std::remove_reference_t<AnyT>, std::remove_reference_t<Ts>...>)
        constexpr decltype(auto) get()
            noexcept(noexcept(static_cast<std::remove_reference_t<AnyT> &>(*this)))
        {
            return static_cast<std::remove_reference_t<AnyT> &>(*this);
        }

        template <typename AnyT>
            requires (requires(TUniqueTuple ut) { static_cast<std::remove_reference_t<AnyT> const&>(ut); })
        constexpr decltype(auto) as() const
        {
            return static_cast<std::remove_reference_t<AnyT> const &>(*this);
        }

        template <typename AnyT>
            requires (not requires(TUniqueTuple ut) { static_cast<std::remove_reference_t<AnyT> const&>(ut); })
        constexpr decltype(auto) as() const
        {
            return static_cast<AnyT>(*this);
        }

        template <typename AnyT>
            requires (requires(TUniqueTuple ut) { static_cast<std::remove_reference_t<AnyT>&>(ut); })
        constexpr decltype(auto) as()
        {
            return static_cast<std::remove_reference_t<AnyT>&>(*this);
        }

        template <typename AnyT>
            requires (not requires(TUniqueTuple ut) { static_cast<std::remove_reference_t<AnyT> const&>(ut); })
        constexpr decltype(auto) as()
        {
            return static_cast<AnyT>(*this);
        }

        template <typename AnyT>
        constexpr std::strong_ordering operator<=>(AnyT const & other) const noexcept { return *this <=> other; }

        template <typename AnyT>
        constexpr std::strong_ordering operator<=>(AnyT const& other) noexcept { return *this <=> other; }
    };

    template <typename... Ts> requires(sib::is_unique_v<Ts...>) struct MakeUniqueTupleSpec;

    template <typename... Ts>
        requires(sib::is_unique_v<Ts...>)
    struct MakeUniqueTupleSpec<Ts...>
    {
        using type = instantiate_templ_t<TUniqueTuple, types_sequence_t<type_pack<Ts...>>>;
    };

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    struct MakeUniqueTupleSpec<TsTempl<Ts...>>
    {
        using type = instantiate_templ_t<TUniqueTuple, types_sequence_t<TsTempl<Ts...>>>;
    };

    template <typename... Ts> using MakeUniqueTuple = typename MakeUniqueTupleSpec<Ts...>::type;

    template <typename... Args>
    constexpr auto make_unique_tuple(Args&&... args)
        noexcept(noexcept(MakeUniqueTuple<std::remove_reference_t<Args>...>(std::forward<Args>(args)...)))
    {
        return MakeUniqueTuple<std::remove_reference_t<Args>...>(std::forward<Args>(args)...);
    }

} // namespace sib
