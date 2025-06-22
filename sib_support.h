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

    /* https://stackoverflow.com/a/56766138/23601704 */

    template <typename T>
    consteval auto type_name() noexcept
    {
        std::string_view name, prefix, suffix;
        #ifdef __clang__
            name = __PRETTY_FUNCTION__;
            prefix = "auto sib::type_name() [T = ";
            suffix = "]";
        #elif defined(__GNUC__)
            name = __PRETTY_FUNCTION__;
            prefix = "consteval auto sib::type_name() [with T = ";
            suffix = "]";
        #elif defined(_MSC_VER)
            name = __FUNCSIG__;
            prefix = "auto __cdecl sib::type_name<";
            suffix = ">(void) noexcept";
        #else
            static_assert(false, "Unsupported compiler!");
        #endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }

    template <typename T>
    consteval auto type_name(T&&) noexcept
    {
        return type_name<T>();
    }

    template <typename T>
    struct TTypeInfo {
    private:
        using rr_type = std::remove_reference_t<T>;
    public:
        TTypeInfo() {};
        TTypeInfo(T&) {};

        static constexpr auto is_const = std::is_const_v     <rr_type>;
        static constexpr auto is_ref   = std::is_reference_v <T      >;
        static constexpr auto is_arr   = std::is_array_v     <rr_type>;

        static std::string _low_name()
        {
            #define _SIB_IF_ELSE_(type) if constexpr (std::is_same_v<std::remove_cvref_t<T>, type>) { return #type; } else

            _SIB_IF_ELSE_(std::vector<bool>)
            _SIB_IF_ELSE_(std::vector<char>)
            _SIB_IF_ELSE_(std::vector<short>)
            _SIB_IF_ELSE_(std::vector<int>)
            _SIB_IF_ELSE_(std::vector<long>)
            _SIB_IF_ELSE_(std::vector<long long>)
            _SIB_IF_ELSE_(std::vector<float>)
            _SIB_IF_ELSE_(std::vector<double>)
            _SIB_IF_ELSE_(std::string)
            _SIB_IF_ELSE_(std::wstring)
            {
                return typeid(std::remove_extent_t<std::remove_cvref_t<T>>).name() ;
            }

            #undef _SIB_IF_ELSE_
        }

        static std::string _const_symbol()
        {
            if constexpr (is_const) { return " const"; }
            else                    { return ""; }
        }

        static std::string _ref_symbol()
        {
            if constexpr (not is_ref) { return ""; }
            else
            {
                if constexpr (std::is_lvalue_reference_v<T>)
                {
                    if constexpr (is_arr) { return " (&)"; }
                    else                  { return " &"  ; }
                }
                else
                {
                    if constexpr (is_arr) { return " (&&)"; }
                    else                  { return " &&"  ; }
                }
            }
        };



        template <typename _T>
        struct _array_size_symbol { static std::string get() { return ""; } };

        template <typename _T>
        struct _array_size_symbol<_T[]> { static std::string get() { return " []"; } };

        template <typename _T, size_t _N>
        struct _array_size_symbol<_T[_N]> { static std::string get() { return " [" + std::to_string(_N) + "]"; } };



    public:
        static std::string full_name()
        {
            std::string res{ _low_name() };
            res = res + _const_symbol();
            res += _ref_symbol();
            res += _array_size_symbol<rr_type>::get();
            return res;
        }
    };



// ----------------------------------------------------------------------------------- type traits

    template <typename T, typename... Ts>
    struct First
    {
        using type = T;
    };

    template <typename... Ts>
    using First_t = typename First<Ts...>::type;

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

    template <typename T, typename... Ts> constexpr bool  is_any_of_v = (std::is_same_v<T, Ts> or ...);
    template <typename T, typename... Ts> constexpr bool not_any_of_v = not is_any_of_v<T, Ts>;

    template <typename T, typename... Ts> struct  is_any_of : std::bool_constant< is_any_of_v<T, Ts...>> {};
    template <typename T, typename... Ts> struct not_any_of : std::bool_constant<not_any_of_v<T, Ts...>> {};

    template <typename T, typename... Ts> concept     AnyOf =  is_any_of_v<T, Ts...>;
    template <typename T, typename... Ts> concept not_AnyOf = not_any_of_v<T, Ts...>;



    // convertible

    template <typename From, typename... To>
    constexpr bool  is_convertible_from_to_v = (std::is_convertible_v<From, To> or ...);

    template <typename From, typename... To>
    constexpr bool not_convertible_from_to_v = not is_convertible_from_to_v<From, To...>;

    template <typename From, typename... To> struct  is_convertible_form_to : std::bool_constant< is_convertible_from_to_v<From, To...>> {};
    template <typename From, typename... To> struct not_convertible_form_to : std::bool_constant<not_convertible_from_to_v<From, To...>> {};

    template <typename From, typename... To> concept     ConvertibleFromTo =  is_convertible_from_to_v<From, To...>;
    template <typename From, typename... To> concept not_ConvertibleFromTo = not_convertible_from_to_v<From, To...>;



    template <typename To, typename... From>
    constexpr bool  is_convertible_to_from_v = (std::is_convertible_v<From, To> or ...);

    template <typename To, typename... From>
    constexpr bool not_convertible_to_from_v =  not is_convertible_to_from_v<To, From...>;

    template <typename To, typename... From> struct  is_convertible_to_from : std::bool_constant< is_convertible_to_from_v<To, From...>> {};
    template <typename To, typename... From> struct not_convertible_to_from : std::bool_constant<not_convertible_to_from_v<To, From...>> {};

    template <typename To, typename... From> concept     ConvertibleToFrom =  is_convertible_to_from_v<To, From...>;
    template <typename To, typename... From> concept not_ConvertibleToFrom = not_convertible_to_from_v<To, From...>;



    template <typename From, typename... To>
    struct select_conversion_from_to {};

    template <typename From, typename... To>
    using select_conversion_from_to_t = typename select_conversion_from_to<From, To...>::type;

    template <typename From, typename To, typename... To_others>
        requires(is_convertible_from_to_v<From, To>)
    struct select_conversion_from_to<From, To, To_others...> { using type = To; };

    template <typename From, typename To, typename... To_others>
        requires(is_convertible_from_to_v<From, To_others...>)
    struct select_conversion_from_to<From, To, To_others...> { using type = select_conversion_from_to_t<From, To_others...>; };



    template <typename To, typename... From>
    struct select_conversion_to_from {};

    template <typename To, typename... From>
    using select_conversion_to_from_t = typename select_conversion_to_from<To, From...>::type;

    template <typename To, typename From, typename... From_others>
        requires(is_convertible_to_from_v<To, From>)
    struct select_conversion_to_from<To, From, From_others...> { using type = From; };

    template <typename To, typename From, typename... From_others>
        requires(is_convertible_to_from_v<To, From_others...>)
    struct select_conversion_to_from<To, From, From_others...> { using type = select_conversion_to_from_t<To, From_others...>; };



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
    using container_elem_t = decltype( * std::begin(std::declval<Cont&>()) );



    // enum

    template <typename T> constexpr bool  is_enum_v = std::is_enum_v<T>;
    template <typename T> constexpr bool not_enum_v =  not is_enum_v<T>;

    template <typename T> struct  is_enum : std::bool_constant< is_enum_v<T>> {};
    template <typename T> struct not_enum : std::bool_constant<not_enum_v<T>> {};

    template <typename T> concept     Enum =  is_enum_v<T>;
    template <typename T> concept not_Enum = not_enum_v<T>;



    template <typename T> struct is_enum_class : std::false_type {};

    template <Enum E>
        requires (not std::is_convertible_v<E, std::underlying_type_t<std::remove_cvref_t<E>>>)
    struct is_enum_class<E> : std::true_type {};

    template <typename T> struct not_enum_class : std::bool_constant<not is_enum_class<T>::value> {};

    template <typename T> constexpr bool  is_enum_class_v =  is_enum_class<T>::value;
    template <typename T> constexpr bool not_enum_class_v = not_enum_class<T>::value;

    template <typename T> concept     EnumClass =  is_enum_class_v<T>;
    template <typename T> concept not_EnumClass = not_enum_class_v<T>;
    
    
    
    // function

    template <typename T> constexpr bool  is_function_v = std::is_function_v<T>;
    template <typename T> constexpr bool not_function_v =  not is_function_v<T>;

    template <typename T> struct  is_function : std::bool_constant< is_function_v<T>> {};
    template <typename T> struct not_function : std::bool_constant<not_function_v<T>> {};

    template <typename T> concept     Function =  is_function_v<T>;
    template <typename T> concept not_Function = not_function_v<T>;



    template <typename T>
    constexpr bool is_like_function_v = requires (T f) { std::function(f); };

    template <typename T>
    constexpr bool not_like_function_v = not is_like_function_v<T>;

    template <typename T> struct  is_like_function : std::bool_constant< is_like_function_v<T>> {};
    template <typename T> struct not_like_function : std::bool_constant<not_like_function_v<T>> {};

    template <typename T> concept     LikeFunction =  is_like_function_v<T>;
    template <typename T> concept not_LikeFunction = not_like_function_v<T>;



    // pointer

    template <typename T>
    constexpr bool is_like_nullptr_v = std::is_constructible_v<std::nullptr_t, T>;

    template <typename T>
    constexpr bool not_like_nullptr_v = not is_like_nullptr_v<T>;

    template <typename T> struct  is_like_nullptr : std::bool_constant< is_like_nullptr_v<T>> {};
    template <typename T> struct not_like_nullptr : std::bool_constant<not_like_nullptr_v<T>> {};

    template <typename T> concept     LikeNullptr =  is_like_nullptr_v<T>;
    template <typename T> concept not_LikeNullptr = not_like_nullptr_v<T>;



    template <typename T> constexpr bool  is_pointer_v = std::is_pointer_v<T>;
    template <typename T> constexpr bool not_pointer_v =  not is_pointer_v<T>;

    template <typename T> struct  is_pointer : std::bool_constant< is_pointer_v<T>> {};
    template <typename T> struct not_pointer : std::bool_constant<not_pointer_v<T>> {};

    template <typename T> concept     Pointer =  is_pointer_v<T>;
    template <typename T> concept not_Pointer = not_pointer_v<T>;



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

    template <typename T>
    struct not_like_pointer : std::bool_constant<is_like_pointer<T>::value> {};

    template <typename T> constexpr bool  is_like_pointer_v =  is_like_pointer<T>::value;
    template <typename T> constexpr bool not_like_pointer_v = not_like_pointer<T>::value;

    template <typename T> concept     LikePointer =  is_like_pointer_v<T>;
    template <typename T> concept not_LikePointer = not_like_pointer_v<T>;



    template <LikePointer P>
        requires (not_like_nullptr_v<P>)
    using base_pointer_type = decltype(std::declval<P>().operator*());



    template <typename T>
    constexpr bool may_be_indirect_v =
            is_like_pointer_v<T>
        and not_function_v<T>
        and requires(T t) { *t; }
    ;

    template <typename T>
    constexpr bool cant_be_indirect_v = not may_be_indirect_v<T>;
        
    template <typename T> struct  may_be_indirect : std::bool_constant< may_be_indirect_v<T>> {};
    template <typename T> struct cant_be_indirect : std::bool_constant<cant_be_indirect_v<T>> {};

    template <typename T> concept     Indirected =  may_be_indirect_v<T>;
    template <typename T> concept not_Indirected = cant_be_indirect_v<T>;



    template <Indirected P>
    using base_indirection_type = std::remove_reference_t<decltype(*std::declval<P>())>;



    // array

    template <typename T> constexpr bool  is_array_v = std::is_array_v<T>;
    template <typename T> constexpr bool not_array_v =  not is_array_v<T>;

    template <typename T> using   is_array = std::is_array<T>;
    template <typename T> struct not_array : std::bool_constant<not_array_v<T>> {};

    template <typename T> concept     Array =  is_array_v<T>;
    template <typename T> concept not_Array = not_array_v<T>;



    // char

    template <typename T>
    constexpr bool is_char_v = is_any_of_v< std::remove_const_t<T>, char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t >;

    template <typename T>
    constexpr bool not_char_v = not is_char_v<T>;

    template <typename T>
    struct is_char : std::bool_constant<is_char_v<T>> {};

    template <typename T> concept     Char =  is_char_v<T>;
    template <typename T> concept not_Char = not_char_v<T>;



    // like_string

    template <typename T>
    struct is_like_string : std::false_type {};

    template <Container T>
        requires ( is_char_v< std::remove_cvref_t< container_elem_t<T> > > )
    struct is_like_string<T> : std::true_type {};

    template <typename T>
    struct not_like_string : std::bool_constant<not is_like_string<T>::value>{};

    template <typename T> constexpr bool  is_like_string_v =  is_like_string<T>::value;
    template <typename T> constexpr bool not_like_string_v = not_like_string<T>::value;

    template <typename T> concept     LikeString =  is_like_string_v<T>;
    template <typename T> concept not_LikeString = not_like_string_v<T>;



    // unique type list

    template <typename T, typename... Ts>
    constexpr bool is_unique_v =
        not (std::is_convertible_v<T, Ts> || ...)
    and not (std::is_convertible_v<Ts, T> || ...)
    and (is_unique_v<Ts...>)
    ;

    template <typename T>
    constexpr bool is_unique_v<T> = true;



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

// ----------------------------------------------------------------------------------- working with console

    using TKeyCode = int;

    template <std::integral Int>
    struct ByteConcat
    {
        unsigned char bytes[sizeof(Int)];
        constexpr Int value() const
        {
            Int res = 0;
            for (int i = sizeof(Int) - 1; i >= 0; --i)
            {
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

