#pragma once

#include <iostream>
#include <concepts>
#include <string>
#include <utility>

namespace sib {

    template <typename...>
    using always_false = std::false_type;

    template <typename... Ts>
    constexpr bool always_false_v = always_false<Ts...>::value;

    template <typename...>
    using always_void_t = void;



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



} // namespace sib

