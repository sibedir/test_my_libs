    #pragma once

#include <cstdlib>
#include <concepts>
#include <string>
#include <functional>
#include <set>
#include <map>

namespace sib {

    template <typename>
    using always_false = std::false_type;

    template <typename T>
    constexpr bool always_false_v = always_false_v<T>::value;

    template <typename>
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



// ----------------------------------------------------------------------------------- type info

    template <typename T>
    struct TTypeInfo {
    private:
        using rr_type = std::remove_reference_t<T>;
    public:
        TTypeInfo() {};
        TTypeInfo(T&) {};

        static constexpr auto is_const = std::is_const_v    <rr_type>;
        static constexpr auto is_ref = std::is_reference_v<T>;
        static constexpr auto is_arr = std::is_array_v    <rr_type>;
        static std::string _low_name() {
            return typeid(std::remove_extent_t<std::remove_cvref_t<T>>).name();
        }

        static std::string _const_symbol() {
            if (is_const) { return " const"; }
            return {};
        }

        static std::string _ref_symbol() {
            if (not is_ref) { return ""; }

            std::string res;
            if (std::is_lvalue_reference_v<T>) res = "&";
            else                               res = "&&";

            if (is_arr) res = " (" + res + ")";
            else        res = " " + res;

            return res;
        };



        template <typename _T>
        struct _array_size_symbol { static std::string get() { return ""; } };

        template <typename _T>
        struct _array_size_symbol<_T[]> { static std::string get() { return "[]"; } };

        template <typename _T, size_t _N>
        struct _array_size_symbol<_T[_N]> { static std::string get() { return "[" + std::to_string(_N) + "]"; } };



    public:
        static std::string full_name() {
            std::string res = _low_name();
            if (is_const) { res += " const"; }
            res += _ref_symbol();
            res += _array_size_symbol<rr_type>::get();
            return res;
        }
    };



// ----------------------------------------------------------------------------------- type_traits

    // instantiation

    template <template <typename...> typename, typename...>
    constexpr bool is_instantiation_v = false;

    template <template <typename...> typename Templ, typename... Ts>
    constexpr bool is_instantiation_v<Templ, Templ<Ts...>> = true;

    template <template <typename...> typename Templ, typename... Ts>
    using is_instantiation = std::bool_constant<is_instantiation_v<Templ, Ts...>>;

    template <typename T, template <typename...> typename Templ>
    concept Instantiation = is_instantiation_v<Templ, T>;

    template <typename T, template <typename...> typename Templ>
    concept not_Instantiation = not is_instantiation_v<Templ, T>;



    // any of ...

    template <typename T, typename... Ts>
    constexpr bool is_any_of_v = (std::is_same_v<T, Ts> or ...);

    template <typename T, typename... Ts>
    using is_any_of = std::bool_constant<is_any_of_v<T, Ts...>>;

    template <typename T, typename... Ts>
    concept Any_of = is_any_of_v<T, Ts...>;

    template <typename T, typename... Ts>
    concept not_Any_of = not is_any_of_v<T, Ts...>;



    // container

    template <typename T>
    // до лучших времён
    // concept Container = requires (T v) { for (auto it : v) {} } );
    concept Container =
        requires (T& v)
        {
            { std::begin(v) == std::end(v) } -> std::same_as<bool>;
        }
    and
        requires (decltype(std::begin(std::declval<T&>())) it)
        {
            { *it };
            { ++it } -> std::same_as<decltype(it)&>;
        }
    ;

    template <typename T>
    struct is_container : std::false_type {};

    template <Container T>
    struct is_container<T> : std::true_type {};

    template <typename T>
    constexpr bool is_container_v = is_container<T>::value;

    template <typename T>
    concept not_Container = not is_container_v<T>;



    template <Container Cont>
    using container_elem_t = decltype(std::declval<Cont>().begin().operator *());



    // enum

    template <typename T>
    using is_enum = std::is_enum<T>;

    template <typename T>
    constexpr bool is_enum_v = std::is_enum_v<T>;

    template <typename T>
    concept Enum = is_enum_v<T>;

    template <typename T>
    concept not_Enum = not is_enum_v<T>;



    template <typename T>
    concept Enum_class = std::is_enum_v<T> and (not std::is_convertible_v<T, std::underlying_type_t<T>>);

    template <typename T>
    constexpr bool is_enum_class_v = false;

    template <Enum_class E>
    constexpr bool is_enum_class_v<E> = true;

    template <typename T>
    using is_enum_class = std::bool_constant<is_enum_class_v<T>>;

    template <typename T>
    concept not_Enum_class = not is_enum_class_v<T>;



    // pointer

    template <typename T>
    constexpr bool is_like_nullptr_v = std::is_constructible_v<std::nullptr_t, T>;

    template <typename T>
    using is_like_nullptr = std::bool_constant<is_like_nullptr_v<T>>;

    template <typename T>
    concept Like_nullptr = is_like_nullptr_v<T>;

    template <typename T>
    concept not_Like_nullptr = not is_like_nullptr_v<T>;



    template <typename T> class is_like_pointer;

    template <typename T>
    concept Like_pointer =
        std::is_pointer_v<T>
     or (not std::is_function_v<T> and std::is_convertible_v<T, void const *>)
     or (
            is_like_pointer<decltype(std::declval<T>().operator->())>::value
            and
            std::is_same_v<
                std::remove_pointer_t< decltype(std::declval<T>().operator->()) >,
                                       decltype(std::declval<T>().operator *()) 
            >
        )
    ;

    template <typename T>
    constexpr bool is_like_pointer_v = false;

    template <Like_pointer T>
    constexpr bool is_like_pointer_v<T> = true;

    template <typename T>
    class is_like_pointer : public std::bool_constant<is_like_pointer_v<T>> {};

    template <typename T>
    concept not_Like_pointer = not is_like_pointer_v<T>;



    template <Like_pointer P>
        requires (not is_like_nullptr_v<P>)
    using base_pointer_type = decltype(std::declval<P>().operator*());



    template <typename T>
    concept Indirected
        = is_like_pointer_v<T>
        and not std::is_function_v<T>
        and requires(T t) { *t; }
    ;
    
    template <typename T>
    constexpr bool may_be_indirect_v = false;

    template <Indirected T>
    constexpr bool may_be_indirect_v<T> = true;

    template <typename T>
    using may_be_indirect = std::bool_constant<may_be_indirect_v<T>>;

    template <typename T>
    concept not_Indirected = not may_be_indirect_v<T>;



    template <Indirected P>
    struct indirection_spec {
        using base_type = std::remove_reference_t<decltype(*std::declval<P>())>;
        using ptr_type = base_type*;
    };



    // function


    template <typename T>
    constexpr bool is_function_v = std::is_function_v<T>;

    template <typename T>
    using is_function = std::is_function<T>;

    template <typename T>
    concept Function = is_function_v<T>;

    template <typename T>
    concept not_Function = not is_function_v<T>;

    template <typename T>
    constexpr bool is_like_function_v = requires (T f) { std::function(f); };

    template <typename T>
    using is_like_function = std::bool_constant<is_like_function_v<T>>;

    template <typename T>
    concept Like_function = is_like_function_v<T>;
        
    template <typename T>
    concept not_Like_function = not is_like_function_v<T>;



    // char

    template <typename T>
    constexpr bool is_char_v = is_any_of_v< T, char, wchar_t, char8_t, char16_t, char32_t >;

    template <typename T>
    struct is_char : std::bool_constant<is_char_v<T>> {};

    template <typename T>
    concept Char = is_char_v<T>;

    template <typename T>
    concept not_Char = not is_char_v<T>;



    // string

    template <typename T>
    struct is_basic_string : std::false_type {};

    template <typename T>
        requires (
            std::is_same_v<
                T,
                std::basic_string<
                    typename T::value_type,
                    typename T::traits_type,
                    typename T::allocator_type
                >
            >
        )
    struct is_basic_string<T> : std::bool_constant<is_char_v<typename T::value_type>> {};

    template <typename T>
    constexpr bool is_basic_string_v = is_basic_string<T>::value;

    template <typename T>
    concept Basic_string = is_basic_string_v<T>;

    template <typename T>
    concept not_Basic_string = not is_basic_string_v<T>;



// ----------------------------------------------------------------------------------- rand

    /* random in [0, 1) */ extern inline double rand();
    /* random in [0, X) */ extern inline double rand(double);



// ----------------------------------------------------------------------------------- extrude

    template<std::integral Int>
    inline Int post_dev(Int& val, Int divisor) noexcept {
        Int res = val;
        val /= divisor;
        return res;
    }

    template<std::integral Int>
    inline Int dev_ret_mod(Int& val, Int divisor) noexcept {
        return post_dev(val, divisor) % divisor;
    }

    template<std::integral Int>
    inline Int extrude(Int& val, char bits) noexcept {
        return dev_ret_mod(val, static_cast<Int>(Int(1) << bits));
    }

    /*
    template<std::integral Int>
    inline Int extrude(Int& val, char bits) noexcept {
        Int divisor = (Int(1) << bits);
        Int res = val % divisor;
        val /= divisor;
        return res;
    }
    //*/
    
    /*
    template<std::integral Int>
    inline Int extrude(Int& val, char bits) noexcept(noexcept(std::div(val, std::declval<Int>()))) {
        auto res = std::div(val, Int(1) << bits);
        val = res.quot;
        return res.rem;
    }
    //*/

// ----------------------------------------------------------------------------------- working with console

    using TKeyCode = int;

    template <std::integral Int>
    struct ByteConcat {
        unsigned char bytes[sizeof(Int)];
        constexpr Int value() const {
            Int res = 0;
            for (int i = sizeof(Int) - 1; i >= 0; --i) {
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

    inline constexpr TKeyCode KC_INSERT    = (ByteConcat<TKeyCode>{  KC_NUM_INSERT   , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_DELETE    = (ByteConcat<TKeyCode>{  KC_NUM_DELETE   , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_HOME      = (ByteConcat<TKeyCode>{  KC_NUM_HOME     , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_END       = (ByteConcat<TKeyCode>{  KC_NUM_END      , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_PAGE_UP   = (ByteConcat<TKeyCode>{  KC_NUM_PAGE_UP  , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_PAGE_DOWN = (ByteConcat<TKeyCode>{  KC_NUM_PAGE_DOWN, KC_COMM_2 }).value();

    inline constexpr TKeyCode KC_NUM_LEFT  = 75;
    inline constexpr TKeyCode KC_NUM_RIGHT = 77;
    inline constexpr TKeyCode KC_NUM_UP    = 72;
    inline constexpr TKeyCode KC_NUM_DOWN  = 80;

    inline constexpr TKeyCode KC_LEFT  = (ByteConcat<TKeyCode>{  KC_NUM_LEFT , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_RIGHT = (ByteConcat<TKeyCode>{  KC_NUM_RIGHT, KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_UP    = (ByteConcat<TKeyCode>{  KC_NUM_UP   , KC_COMM_2 }).value();
    inline constexpr TKeyCode KC_DOWN  = (ByteConcat<TKeyCode>{  KC_NUM_DOWN , KC_COMM_2 }).value();

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

