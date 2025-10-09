#pragma once

#include <utility>
#include <type_traits>
#include <string>
#include <cstdint>

namespace sib {

    template <typename...> using always_true  = ::std::true_type;
    template <typename...> using always_false = ::std::false_type;

    template <typename... Ts> constexpr bool always_true_v  = always_true <Ts...>::value;
    template <typename... Ts> constexpr bool always_false_v = always_false<Ts...>::value;

    template <typename...>
    using always_void_t = void;

    template <auto MSG>
    struct error { inline static constexpr auto msg = MSG; };

    #define SIB_STR(...) #__VA_ARGS__
    #define SIB_CONCAT_INNER(a, b) a##b
    #define SIB_CONCAT(a, b) SIB_CONCAT_INNER(a, b)

    #define SIB_STRINGISE_IMPL(x) #x
    #define SIB_STRINGISE(x) SIB_STRINGISE_IMPL(x)


    #define SIB_MAKE_LITERAL(CharT, txt) []             \
    {                                                   \
        if constexpr (std::is_same_v<CharT, char>)      \
            return (const char*)  txt;                  \
        else                                            \
        if constexpr (std::is_same_v<CharT, wchar_t>)   \
            return (const wchar_t*) L##txt;             \
        else                                            \
        if constexpr (std::is_same_v<CharT, char8_t>)   \
            return (const char8_t*) u8##txt;            \
        else                                            \
        if constexpr (std::is_same_v<CharT, char16_t>)  \
            return (const char16_t*) u##txt;            \
        else                                            \
        if constexpr (std::is_same_v<CharT, char32_t>)  \
            return (const char32_t*) U##txt;            \
    }()                                                 \



    // ----------------------------------------------------------------------------------- scope_guard

    template <typename F>
        requires ::std::is_nothrow_invocable_v<F>
    class scope_guard {
    public:
        explicit constexpr scope_guard(F&& func)
            noexcept(std::is_nothrow_move_constructible_v<F>)
            : func_(std::forward<F>(func))
            , active_(true)
        {}

        constexpr scope_guard(scope_guard&& other)
            noexcept(std::is_nothrow_move_constructible_v<F>)
            : func_(std::move(other.func_))
            , active_(other.active_)
        {
            other.active_ = false;
        }

        scope_guard(const scope_guard&) = delete;
        scope_guard& operator=(const scope_guard&) = delete;

        ~scope_guard() noexcept { if (active_) func_(); }

        void dismiss() noexcept { active_ = false; }

    private:
        F func_;
        bool active_;
    };

    template <typename F>
    [[nodiscard]] constexpr inline scope_guard<F> make_scope_guard(F&& f) {
        return scope_guard<F>(std::forward<F>(f));
    }

    #define SIB_SCOPE_GUARD(...)                                        \
        auto SIB_CONCAT(sib_scope_guard_guard, __COUNTER__) =           \
            ::sib::make_scope_guard( [&]() noexcept { __VA_ARGS__ } );  \



    // ----------------------------------------------------------------------------------- forward_like

    template<class T, class U>
    constexpr auto&& forward_like(U&& x) noexcept
    {
        constexpr bool is_adding_const = ::std::is_const_v<::std::remove_reference_t<T>>;
        if constexpr (std::is_lvalue_reference_v<T&&>)
        {
            if constexpr (is_adding_const)
                return ::std::as_const(x);
            else
                return static_cast<U&>(x);
        }
        else
        {
            if constexpr (is_adding_const)
                return ::std::move(std::as_const(x));
            else
                return ::std::move(x);
        }
    }



    // ----------------------------------------------------------------------------------- pointer

    template <typename T>
    constexpr auto ptr_to_int(T* ptr) noexcept
    {
        return reinterpret_cast<::std::uintptr_t const>(ptr);
    }



    // ----------------------------------------------------------------------------------- rand

    /* random in [0, 1) */ double rand();
    /* random in [0, X) */ double rand(double);



    // ----------------------------------------------------------------------------------- extrude

    template<::std::integral Int>
    Int post_dev(Int& val, Int divisor) noexcept
    {
        Int res = val;
        val /= divisor;
        return res;
    }

    template<::std::integral Int>
    Int dev_ret_mod(Int& val, Int divisor) noexcept
    {
        return post_dev(val, divisor) % divisor;
    }

    template<::std::integral Int>
    Int extrude(Int& val, char bits) noexcept
    {
        return dev_ret_mod(val, static_cast<Int>(Int(1) << bits));
    }

    /*
    template<::std::integral Int>
    Int extrude(Int& val, char bits) noexcept
    {
        Int divisor = (Int(1) << bits);
        Int res = val % divisor;
        val /= divisor;
        return res;
    }

    template<::std::integral Int>
    Int extrude(Int& val, char bits) noexcept(noexcept(std::div(val, ::std::declval<Int>())))
    {
        auto res = ::std::div(val, Int(1) << bits);
        val = res.quot;
        return res.rem;
    }
    */

} // namespace sib
