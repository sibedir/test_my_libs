#pragma once

#include "sib_support.h"
#include "sib_wrapper.h"

namespace sib {

    template <typename... Ts>
        requires(sib::is_unique_v<Ts...> and sib::is_sorted_v<Ts...>)
    class TUniqueTuple : public TWrapper<Ts> ...
    {
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
        template <template <typename...> typename TsTempl>
            requires(std::is_base_of_v<container_of_types, TsTempl<>>)
        using types = TsTempl<Ts...>;

        template <ConvertibleFromTo<Ts...>... Args>
        TUniqueTuple(Args&&... args) noexcept : conversion_type_from<Args>(args) ... {}

        template <ConvertibleFromTo<Ts...> AnyT>
        constexpr auto operator= (AnyT&& other)
            noexcept(noexcept( conversion_type_from<AnyT>::operator=(std::declval<AnyT>()) ))
        { return conversion_type_from<AnyT>::operator=(std::forward<AnyT>(other)); }

        template <AnyOf<Ts...> T> constexpr T const & as() const noexcept { return *this; };
        template <AnyOf<Ts...> T> constexpr T       & as()       noexcept { return *this; };
    };

    template <typename... Ts> requires(sib::is_unique_v<Ts...>) struct MakeUniqueTupleSpec;

    template <typename... Ts>
        requires(sib::is_unique_v<Ts...>)
    struct MakeUniqueTupleSpec<Ts...>
    {
        using type = specialization_templ_t<TUniqueTuple, types_sequence_t<type_list<Ts...>>>;
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
        noexcept(noexcept(MakeUniqueTuple<Args...>(args...)))
    {
        return MakeUniqueTuple<Args...>(args...);
    }

} // namespace sib
