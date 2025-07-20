#pragma once

#include "sib_support.h"
#include "sib_wrapper.h"

namespace sib {

    //template <typename Dest, class Source>
    //struct appointer {
    //    assignment_operator_result_t<Dest, Source> operator=(Source && val) {
    //        return *static_cast<Dest*>(this) = std::forward<Source>(val);
    //    }
    //};

    template <typename... Ts>
        requires(sib::is_unique_v<Ts...> and sib::is_sorted_v<Ts...>)
    class TUniqueTuple :
        //public appointer<TUniqueTuple<Ts...>, Ts> ...,
        public TWrapper<Ts> ...
    {
    private:
        template <ConvertibleFromToSome<Ts...> AnyT>
        using conversion_base_type_from = convert_from_tooneof_t<AnyT, Ts...>;

        template <ConvertibleFromToSome<Ts...> AnyT>
        using conversion_type_from = TWrapper<conversion_base_type_from<AnyT>>;

        template <ConvertibleToFromSome<Ts...> AnyT>
        using conversion_base_type_to = convert_to_fromoneof_t<AnyT, Ts...>;

        template <ConvertibleToFromSome<Ts...> AnyT>
        using conversion_type_to = TWrapper<conversion_base_type_to<AnyT>>;

        template <ConstructibleFromToSome<Ts...> AnyT>
        using construction_base_type_from = construct_from_tooneof_t<AnyT, Ts...>;

        template <ConstructibleFromToSome<Ts...> AnyT>
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
        using veritable_types = TsTempl< TWrapper<Ts>...>;

        //using appointer<TUniqueTuple<Ts...>, Ts>::operator= ...;

        template <ConstructibleFromToOneOf<Ts...>... AnyTs>
            requires( sizeof...(Ts) >= sib::types_info<sib::type_pack<construction_type_from<AnyTs>...>>::count)
        TUniqueTuple(AnyTs&&... other)
            noexcept(noexcept_constr_ts<AnyTs...>)
            : construction_type_from<AnyTs>(std::forward<AnyTs>(other)) ...
        {}

        //TUniqueTuple() /*requires(std::is_default_constructible_v<TWrapper<Ts>> and ...)*/ = default;
        //TUniqueTuple(TUniqueTuple const & other) = default;
        //TUniqueTuple(TUniqueTuple      && other) = default;
        //TUniqueTuple(TWrapper<Ts> const & ... other) : TWrapper<Ts>(other) ... {}

        //constexpr TUniqueTuple& operator= (TUniqueTuple const & other) = default;
        //constexpr TUniqueTuple& operator= (TUniqueTuple      && other) = default;

        //~TUniqueTuple() = default;

        template <HasAssignmentOperatorFromToOneOf<Ts...> AnyT>
        constexpr assignment_operator_from_tooneof_result<AnyT, Ts...> operator= (AnyT&& other)
            noexcept(noexcept(TWrapper<assignment_operator_from_tooneof_select<AnyT, Ts...>>::operator=(std::declval<AnyT>())))
        { return static_cast<assignment_operator_from_tooneof_select<AnyT, Ts...>&>(*this) = std::forward<AnyT>(other); }

        template <AnyOf<Ts...> T> constexpr T const & get() const noexcept { return *this; }
        template <AnyOf<Ts...> T> constexpr T       & get()       noexcept { return *this; }
    };

    template <typename... Ts> requires(sib::is_unique_v<Ts...>) struct MakeUniqueTupleSpec;

    template <typename... Ts>
        requires(sib::is_unique_v<Ts...>)
    struct MakeUniqueTupleSpec<Ts...>
    {
        using type = specialization_templ_t<TUniqueTuple, types_sequence_t<type_pack<Ts...>>>;
    };

    template <template <typename...> typename TsTempl, typename... Ts>
        requires(std::is_base_of_v<container_of_types, TsTempl<>>)
    struct MakeUniqueTupleSpec<TsTempl<Ts...>>
    {
        using type = specialization_templ_t<TUniqueTuple, types_sequence_t<TsTempl<Ts...>>>;
    };

    template <typename... Ts> using MakeUniqueTuple = typename MakeUniqueTupleSpec<Ts...>::type;

    template <typename... Args>
    constexpr auto make_unique_tuple(Args&&... args)
        noexcept(noexcept(MakeUniqueTuple<std::remove_reference_t<Args>...>(std::forward<Args>(args)...)))
    {
        return MakeUniqueTuple<std::remove_reference_t<Args>...>(std::forward<Args>(args)...);
    }

} // namespace sib
