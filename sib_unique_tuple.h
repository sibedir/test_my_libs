#pragma once

#include "sib_support.h"
#include "sib_wrapper.h"

namespace sib {

    namespace detail {

        template <typename... Ts>
        class _TUniqueTuple_ : public TWrapper<Ts> ...
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
            using pack = type_pack<Ts...>;

            template <ConvertibleFromTo<Ts...>... Args>
            _TUniqueTuple_(Args&&... args) noexcept : conversion_type_from<Args>(args) ... {}

            template <ConvertibleFromTo<Ts...> AnyT>
            constexpr auto operator= (AnyT&& other)
                noexcept(noexcept( conversion_type_from<AnyT>::operator=(std::declval<AnyT>()) ))
            { return conversion_type_from<AnyT>::operator=(std::forward<AnyT>(other)); }

            template <AnyOf<Ts...> T> constexpr T const & as() const noexcept { return *this; };
            template <AnyOf<Ts...> T> constexpr T       & as()       noexcept { return *this; };

        };

    } // detail

    template <typename... Ts>
        requires(sib::is_unique_v<Ts...> and sib::is_sorted_v<Ts...>)
    using TUniqueTuple = detail::_TUniqueTuple_<Ts...>;

    template <typename... Ts>
    using MakeUniqueTuple = specialization_templ_t<detail::_TUniqueTuple_, sorted_type_list_t<Ts...>>;

    template <typename... Args>
    constexpr MakeUniqueTuple<Args...> make_unique_tuple(Args&&... args)
        noexcept(noexcept(MakeUniqueTuple<Args...>(args...)))
    {
        return MakeUniqueTuple<Args...>(args...);
    }

} // namespace sib
