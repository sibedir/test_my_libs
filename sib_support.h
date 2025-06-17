#pragma once

#include <cstdlib>
#include <concepts>
#include <string>
#include <functional>
#include <set>
#include <map>

#include <utility>

namespace sib {

    template <typename>
    using always_false = std::false_type;

    template <typename T>
    constexpr bool always_false_v = always_false<T>::value;

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
        struct _array_size_symbol<_T[]> { static std::string get() { return " []"; } };

        template <typename _T, size_t _N>
        struct _array_size_symbol<_T[_N]> { static std::string get() { return " [" + std::to_string(_N) + "]"; } };



    public:
        static std::string full_name() {
            std::string res = _low_name();
            res += _const_symbol();
            res += _ref_symbol();
            res += _array_size_symbol<rr_type>::get();
            return res;
        }
    };



// ----------------------------------------------------------------------------------- type traits

    // instantiation

    template <template <typename...> typename, typename...>
    struct  is_instantiation : std::false_type {};

    template <template <typename...> typename Templ, typename... Ts>
    struct  is_instantiation<Templ, Templ<Ts...>> : std::true_type {};

    template <template <typename...> typename Templ, typename... Ts>
    struct not_instantiation : std::bool_constant<is_instantiation<Templ, Templ<Ts...>>::value> {};

    template <template <typename...> typename Templ, typename... Ts> constexpr bool  is_instantiation_v =  is_instantiation<Templ, Templ<Ts...>>::value;
    template <template <typename...> typename Templ, typename... Ts> constexpr bool not_instantiation_v = not_instantiation<Templ, Templ<Ts...>>::value;

    template <typename T, template <typename...> typename Templ>  concept     Instantiation =  is_instantiation_v<Templ, T>;
    template <typename T, template <typename...> typename Templ>  concept not_Instantiation = not_instantiation_v<Templ, T>;



    // any of ...

    template <typename T, typename... Ts> constexpr bool  is_any_of_v =     (std::is_same_v<T, Ts> or ...);
    template <typename T, typename... Ts> constexpr bool not_any_of_v = not (std::is_same_v<T, Ts> or ...);

    template <typename T, typename... Ts> struct  is_any_of : std::bool_constant< is_any_of_v<T, Ts...>> {};
    template <typename T, typename... Ts> struct not_any_of : std::bool_constant<not_any_of_v<T, Ts...>> {};

    template <typename T, typename... Ts> concept     Any_of =  is_any_of_v<T, Ts...>;
    template <typename T, typename... Ts> concept not_Any_of = not_any_of_v<T, Ts...>;


    // container

    template <typename T>
    constexpr bool is_container_v =
        // до лучших времён
        // requires ( requires (T v) { for (auto it : v) {} } )
        ( requires (T& v) {
              { std::begin(v) == std::end(v) } -> std::same_as<bool>;
          }
        )
    and
        ( requires (decltype(std::begin(std::declval<T&>())) it) {
              { *it };
              { ++it } -> std::same_as<decltype(it)&>;
          }
        )
    ;

    template <typename T>
    constexpr bool not_container_v = not is_container_v<T>;

    template <typename T> struct  is_container : std::bool_constant< is_container_v<T>> {};
    template <typename T> struct not_container : std::bool_constant<not_container_v<T>> {};

    template <typename T> concept     Container =  is_container_v<T>;
    template <typename T> concept not_Container = not_container_v<T>;



    template <Container Cont>
    using container_elem_t = decltype(std::declval<Cont>().begin().operator *());



    // enum

    template <typename T> constexpr bool  is_enum_v =      std::is_enum_v<T>;
    template <typename T> constexpr bool not_enum_v = (not std::is_enum_v<T>);

    template <typename T> using   is_enum = std::is_enum<T>;
    template <typename T> struct not_enum : std::bool_constant<not_enum_v<T>> {};

    template <typename T> concept     Enum =  is_enum_v<T>;
    template <typename T> concept not_Enum = not_enum_v<T>;



    template <typename T> constexpr bool  is_enum_class_v = (
        ( std::is_enum_v<T>                                       )
    and ( not std::is_convertible_v<T, std::underlying_type_t<T>> )
    );

    template <typename T> constexpr bool not_enum_class_v = not is_enum_class_v<T>;

    template <typename T> struct  is_enum_class : std::bool_constant< is_enum_class_v<T>> {};
    template <typename T> struct not_enum_class : std::bool_constant<not_enum_class_v<T>> {};

    template <typename T> concept     Enum_class =  is_enum_class_v<T>;
    template <typename T> concept not_Enum_class = not_enum_class_v<T>;
    
    
    
    // function

    template <typename T> constexpr bool  is_function_v = std::is_function_v<T>;
    template <typename T> constexpr bool not_function_v = (not std::is_function_v<T>);

    template <typename T> using is_function = std::is_function<T>;

    template <typename T> concept     Function = is_function_v<T>;
    template <typename T> concept not_Function = not_function_v<T>;



    template <typename T>
    constexpr bool is_like_function_v = requires (T f) { std::function(f); };

    template <typename T>
    constexpr bool not_like_function_v = (not is_like_function_v<T>);

    template <typename T> struct  is_like_function : std::bool_constant< is_like_function_v<T>> {};
    template <typename T> struct not_like_function : std::bool_constant<not_like_function_v<T>> {};

    template <typename T> concept     Like_function = is_like_function_v<T>;
    template <typename T> concept not_Like_function = not_like_function_v<T>;



    // pointer

    template <typename T>
    constexpr bool is_like_nullptr_v = std::is_constructible_v<std::nullptr_t, T>;

    template <typename T>
    constexpr bool not_like_nullptr_v = (not is_like_nullptr_v<T>);

    template <typename T>
    struct is_like_nullptr : std::bool_constant<is_like_nullptr_v<T>> {};

    template <typename T> concept     Like_nullptr =  is_like_nullptr_v<T>;
    template <typename T> concept not_Like_nullptr = not_like_nullptr_v<T>;



    template <typename T>
    struct is_like_pointer : std::false_type {};

    template <typename T>
        requires (
            std::is_pointer_v<T>
         or (
                not_function_v<T>
            and
                std::is_convertible_v<T, void const*>
            )
         or (
                is_like_pointer<decltype(std::declval<T>().operator->())>::value
            and
                std::is_same_v<
                    std::remove_pointer_t< decltype(std::declval<T>().operator->()) >,
                                           decltype(std::declval<T>().operator *())
                >
            )
        )
    struct is_like_pointer<T> : std::true_type {};

    template <typename T> constexpr bool  is_like_pointer_v = is_like_pointer<T>::value;
    template <typename T> constexpr bool not_like_pointer_v = (not is_like_pointer<T>::value);

    template <typename T> concept     Like_pointer =  is_like_pointer_v<T>;
    template <typename T> concept not_Like_pointer = not_like_pointer_v<T>;



    template <Like_pointer P>
        requires (not_like_nullptr_v<P>)
    using base_pointer_type = decltype(std::declval<P>().operator*());



    template <typename T>
    constexpr bool may_be_indirect_v =
            ( is_like_pointer_v<T>      )
        and ( not std::is_function_v<T> )
        and ( requires(T t) { *t; }     )
    ;

    template <typename T>
    constexpr bool cant_be_indirect_v = (not may_be_indirect_v<T>);
        
    template <typename T>
    struct may_be_indirect : std::bool_constant<may_be_indirect_v<T>> {};

    template <typename T> concept     Indirected =  may_be_indirect_v<T>;
    template <typename T> concept not_Indirected = cant_be_indirect_v<T>;



    template <Indirected P>
    struct indirection_spec {
        using base_type = std::remove_reference_t<decltype(*std::declval<P>())>;
        using ptr_type = base_type*;
    };



    // char

    template <typename T>
    constexpr bool is_char_v = is_any_of_v< T, char, wchar_t, char8_t, char16_t, char32_t >;

    template <typename T>
    constexpr bool not_char_v = (not is_char_v<T>);

    template <typename T>
    struct is_char : std::bool_constant<is_char_v<T>> {};

    template <typename T> concept     Char =  is_char_v<T>;
    template <typename T> concept not_Char = not_char_v<T>;



    // basic_string

    template <typename T>
    concept Basic_string =
        std::is_same_v<
            T,
            std::basic_string<
                typename T::value_type,
                typename T::traits_type,
                typename T::allocator_type
            >
        >
    ;

    template <typename T>
    constexpr bool is_basic_string_v = false;
        
    template <Basic_string T>
    constexpr bool is_basic_string_v<T> = true;

    template <typename T>
    constexpr bool not_basic_string_v = (not is_basic_string_v<T>);
        
    template <typename T>
    struct is_basic_string : std::false_type {};

    template <typename T> concept not_Basic_string = not_basic_string_v<T>;



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

