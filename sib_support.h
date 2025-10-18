#pragma once

#include <utility>
#include <type_traits>
#include <cstdint>
#include <stdexcept>


namespace sib {

    template <typename...> using always_true  = ::std::true_type;
    template <typename...> using always_false = ::std::false_type;

    template <typename... Ts> constexpr bool always_true_v  = always_true <Ts...>::value;
    template <typename... Ts> constexpr bool always_false_v = always_false<Ts...>::value;

    template <typename...>
    using always_void_t = void;

    template <auto F, auto... Rest>
    inline constexpr auto first_value = F;

    template <auto... Vs>
    inline constexpr auto last_value = (Vs, ...);

    inline constexpr auto sss = last_value<1,2,3>;

    template <auto MSG>
    struct error { inline static constexpr auto msg = MSG; };

    #define SIB_CONCAT_INNER(a, b) a##b
    #define SIB_CONCAT(a, b) SIB_CONCAT_INNER(a, b)

    #define SIB_ARG_STRINGISE_IMPL(...) __VA_ARGS__
    #define SIB_ARG_STRINGISE(...) SIB_ARG_STRINGISE_IMPL(__VA_ARGS__)

    #define SIB_STR_STRINGISE_IMPL(...) #__VA_ARGS__
    #define SIB_STR_STRINGISE(...) SIB_STR_STRINGISE_IMPL(__VA_ARGS__)

    #define SIB_MAKE_LITERAL(CharT, txt) []() constexpr         \
    {                                                           \
        if constexpr (::std::is_same_v<CharT, char>)            \
            return (const char*)  txt"";                        \
        else                                                    \
        if constexpr (::std::is_same_v<CharT, wchar_t>)         \
            return (const wchar_t*) L##txt"";                   \
        else                                                    \
        if constexpr (::std::is_same_v<CharT, char8_t>)         \
            return (const char8_t*) u8##txt"";                  \
        else                                                    \
        if constexpr (::std::is_same_v<CharT, char16_t>)        \
            return (const char16_t*) u##txt"";                  \
        else                                                    \
        if constexpr (::std::is_same_v<CharT, char32_t>)        \
            return (const char32_t*) U##txt"";                  \
    }()                                                         \



    // ----------------------------------------------------------------------------------- EnumSubset

    template <typename Enum, Enum... Allowed>
        requires (std::is_enum_v<Enum>)
    struct constrained_enum_t
    {
        static_assert(sizeof...(Allowed) > 0, "EnumSubset requires at least one allowed value");

        using value_type = Enum;

        Enum value{ first_value<Allowed...> };

        constexpr constrained_enum_t() = default;

        constexpr constrained_enum_t(Enum v)
            : value(v)
        {
            if (!is_valid(v)) throw std::invalid_argument("Invalid enum value for EnumSubset");
        }

        constexpr constrained_enum_t& operator=(Enum v)
        {
            if (!is_valid(v)) throw std::invalid_argument("Invalid enum value for EnumSubset");

            value = v;
            return *this;
        }

        constexpr operator Enum() const { return value; }

        [[nodiscard]] constexpr bool try_set(Enum v)
        {
            if (!is_valid(v)) return false;

            value = v;
            return true;
        }

        static constexpr bool is_valid(Enum v) noexcept {
            return ((v == Allowed) || ...);
        }

        static constexpr std::array<Enum, sizeof...(Allowed)> allowed_values() noexcept {
            return { Allowed... };
        }
    };

    // ----------------------------------------------------------------------------------- Position

    enum class TPosition { Center, Left, Right, Top, Bottom };

    struct TPositionHor : constrained_enum_t<TPosition, TPosition::Left, TPosition::Center, TPosition::Right>
    {
        using base_type = constrained_enum_t<TPosition, TPosition::Left, TPosition::Center, TPosition::Right>;
        using base_type::base_type;

        static constexpr auto Left   = TPosition::Left  ;
        static constexpr auto Center = TPosition::Center;
        static constexpr auto Right  = TPosition::Right ;
    };


    struct TPositionVer : constrained_enum_t<TPosition, TPosition::Top, TPosition::Center, TPosition::Bottom>
    {
        using base_type = constrained_enum_t<TPosition, TPosition::Top, TPosition::Center, TPosition::Bottom>;
        using base_type::base_type;

        static constexpr auto Top    = TPosition::Top   ;
        static constexpr auto Center = TPosition::Center;
        static constexpr auto Bottom = TPosition::Bottom;
    };

    static_assert(TPositionHor::Left   == TPosition::Left);
    static_assert(TPositionHor::Left   != TPosition::Top);
    static_assert(TPositionHor::Center == TPositionVer::Center);
    static_assert(TPositionHor::Right  != TPositionVer::Top);

    static_assert([]() {
        TPosition p = TPosition::Left;
        TPositionHor ph;
        TPositionVer pv;

        ph = p;
        ph = TPosition::Right;
        //pv = ph; // Error
        p = ph;
        //pv = p; // Error
        return not pv.try_set(p);
    }());

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
        auto SIB_CONCAT(sib_scope_guard_, __COUNTER__) =                \
            ::sib::make_scope_guard( [&]() noexcept { __VA_ARGS__ } );  \

    #define SIB_OBJECT_GUARD(...)                                       \
        auto SIB_CONCAT(sib_object_guard_, __COUNTER__) = __VA_ARGS__;  \


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
        return reinterpret_cast<::std::uintptr_t>(ptr);
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
