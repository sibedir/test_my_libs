#pragma once

#include <array>
#include <initializer_list>
#include <type_traits>
#include <utility>

// Note: sib_wrapper library classes behave as if no volotile exists

namespace sib {

    struct TNullPtr;
    template <typename T> class TConst;
    template <typename T> class TValue;
    template <typename T> using TPointer = TValue<T*>;
    template <typename T, size_t N> class TArray;

    template <typename T> struct TWrapperSpec;

    // TWrapper ---------------------------------------------------------------------------

    template <typename T> struct remove_all_wrapers { using type = T; };

    template <typename T>
    using remove_all_wrapers_t = remove_all_wrapers<T>::type;

    template <                    > struct remove_all_wrapers<TNullPtr      > { using type = std::nullptr_t             ; };
    template <typename T          > struct remove_all_wrapers<TValue  <T>   > { using type = remove_all_wrapers_t<T>    ; };
    template <typename T, size_t N> struct remove_all_wrapers<TArray  <T, N>> { using type = remove_all_wrapers_t<T> [N]; };
    
    
    
    template <typename T> struct TWrapperSpec          { using type = TValue<remove_all_wrapers_t<T>>      ; };
    template <typename T> struct TWrapperSpec<T const> { using type = TValue<remove_all_wrapers_t<T>> const; };
    
    template <> struct TWrapperSpec<std::nullptr_t      > { using type = TNullPtr      ; };
    template <> struct TWrapperSpec<std::nullptr_t const> { using type = TNullPtr const; };

    template <typename T> struct TWrapperSpec<T*      > { using type = TPointer<remove_all_wrapers_t<T>>      ; };
    template <typename T> struct TWrapperSpec<T* const> { using type = TPointer<remove_all_wrapers_t<T>> const; };
    
    template <typename T, size_t N> struct TWrapperSpec<T       [N]> { using type = TArray<remove_all_wrapers_t<T>, N>      ; };
    template <typename T, size_t N> struct TWrapperSpec<T const [N]> { using type = TArray<remove_all_wrapers_t<T>, N> const; };
    
    template <class Class> requires (std::is_class_v  <Class>) struct TWrapperSpec<Class      > { using type = Class      ; };
    template <class Class> requires (std::is_class_v  <Class>) struct TWrapperSpec<Class const> { using type = Class const; };

    template <typename Func> requires (std::is_function_v<Func>) struct TWrapperSpec<Func>  { using type = std::function<Func>; };
    
    //template <typename T> struct TWrapperSpec<T&>  { using type = std::remove_reference_t<remove_all_wrapers_t<T>&> ; };
    //template <typename T> struct TWrapperSpec<T&&> { using type = std::remove_reference_t<remove_all_wrapers_t<T>&&>; };
    
    

    template <typename T>
    using TWrapper = typename TWrapperSpec<T>::type;
    
    
    
    template <typename T>
    TWrapper<std::remove_reference_t<T>> to_wrap(T&& source)
    {
        return std::forward<T>(source);
    }
    
    template <typename T, typename... Args>
    TWrapper<std::remove_reference_t<T>> to_wrap(Args&&... args)
    {
        return TWrapper<std::remove_reference_t<T>>(std::forward<Args>(args)... );
    }
    
    
    
    // TNullPtr ---------------------------------------------------------------------------
    
    struct TNullPtr
    {
        using base_type = std::nullptr_t;
    
        TNullPtr(std::nullptr_t = nullptr) {}
        operator std::nullptr_t() const { return nullptr; }
        explicit operator void const * () const { return nullptr; }
        std::nullptr_t operator= (std::nullptr_t) { return nullptr; }
    };
    
    
    
    // Value ------------------------------------------------------------------------------
    
    template <typename T>
    class TValue
    {
        static_assert(always_false<T>::value, "TValue can accept only mulable arithmetic types, enum or pointer.");
    };

    template <typename T>
        requires (
            (!std::is_const_v<T>)
            and
            (std::is_arithmetic_v<T> or std::is_pointer_v<T>)
        )
    class TValue<T>
    {
    public:
        using data_type = T;
    private:
        T data;
    public:
        constexpr TValue(     ) : data{   } {}
        constexpr TValue(T val) : data(val) {}
    
        template <typename AnyT>
        constexpr TValue(TValue<AnyT> other) : data(other) {}
    
        constexpr operator T const & () const { return data; }
        constexpr operator T       & ()       { return data; }

        constexpr T& operator= (T const& other) { return data = other; }

        constexpr T const operator -> () const requires (std::is_pointer_v<T>) { return data; }
        constexpr T       operator -> ()       requires (std::is_pointer_v<T>) { return data; }

        constexpr explicit operator bool () const requires (std::is_pointer_v<T>) { return data; }

        constexpr TValue& operator ++ () requires (std::is_pointer_v<T>) { ++data; return *this; }
        constexpr TValue& operator -- () requires (std::is_pointer_v<T>) { --data; return *this; }

        constexpr T    operator ++ (int) requires (std::is_pointer_v<T>) { return data++; }
        constexpr T    operator -- (int) requires (std::is_pointer_v<T>) { return data--; }

        template <typename AnyT> requires (std::is_pointer_v<T>)
        constexpr explicit operator AnyT const * () const { return static_cast<AnyT const *>(data); }

        template <typename AnyT> requires (std::is_pointer_v<T>)
        constexpr explicit operator AnyT       * ()       { return static_cast<AnyT       *>(data); }

    };

    template <typename Ret, typename... Args>
    class TValue<Ret(*)(Args...)>
    {
    public:
        using data_type = Ret(*)(Args...);
        data_type data;

        constexpr TValue(data_type ptr) : data(ptr) {}

        constexpr operator data_type () const { return data; }
        //constexpr operator std::function<F> () const { return std::function(data); }

        constexpr data_type& operator= (data_type ptr) { return data = ptr; }

        Ret operator()(Args... args) const { return data(std::forward<Args>(args)...); }
    };

    template <Enum E> requires (not std::is_const_v<E>)
    class TValue<E>
    {
    public:

        using data_type = E;
        using underlying_t = std::underlying_type_t<E>;

        E data;
    
        constexpr TValue(     ) : data{}    {}
        constexpr TValue(E val) : data(val) {}

        constexpr explicit TValue(underlying_t val) requires (is_enum_class_v<E>) : data{ val } {}

        constexpr operator E const & () const { return data; }
        constexpr operator E       & ()       { return data; }
    
        template <std::integral Int>
        constexpr explicit operator Int () const { return static_cast<Int>(data); }
    
        constexpr E& operator= (E const& other) { return data = other; }
        constexpr E& operator= (underlying_t val) { return data = static_cast<E>(val); }
    };


    
    template <typename T>
    constexpr bool is_like_enum_v = std::is_enum_v<T>;
    
    template <Enum E>
    constexpr bool is_like_enum_v<TValue<E>> = true;
    
    template <typename T>
    using is_like_enum = std::bool_constant<is_like_enum_v<T>>;
    
    template <typename T>
    concept like_enum = is_like_enum_v<T>;
    
    template <typename T>
    struct underlying_type : std::underlying_type<T> {};

    template <Enum E>
    struct underlying_type<TValue<E>> {
        using type = std::underlying_type_t<E>;
    };

    template <like_enum E>
    using underlying_type_t = typename underlying_type<E>::type;

    template <typename T>
    TValue(T) -> TValue<remove_all_wrapers_t<std::remove_cvref_t<T>>>;
    

    
    // Pointer ----------------------------------------------------------------------------
    
    /*
    template <typename T>
    class TPointer
    {
    private:
    
        using base_type_ = remove_all_wrapers_t<T>;
        using data_type = base_type_*;
        data_type data;
    
    public:
    
        constexpr TPointer() : data{} {}
        constexpr TPointer(data_type ptr) : data(ptr) {}
    
        constexpr operator data_type const & () const { return data; }
        constexpr operator data_type       & ()       { return data; }
    
        template <typename AnyT>
        constexpr explicit operator AnyT* const & () const { return static_cast<AnyT* const &>(data); }
    
        template <typename AnyT>
        constexpr explicit operator AnyT*       & ()       { return static_cast<AnyT*       &>(data); }
    
        constexpr explicit operator bool() const { return data; }
    
        constexpr data_type& operator= (data_type const & ot) { return data = val    ; }
        constexpr data_type& operator= (std::nullptr_t       ) { return data = nullptr; }
    
        template <typename AnyT>
        constexpr data_type& operator= (AnyT* other) { this->data = other; return this->data; }
    
        constexpr T*& operator= (TPointer<rconst_type> const & PTR) { this->data = PTR; return this->data; }
        constexpr T*& operator= (TPointer<rconst_type>      && PTR) { this->data = PTR; return this->data; }
    
        constexpr T*& operator= (TPointer<rconst_type const> const & PTR) { this->data = PTR; return this->data; }
        constexpr T*& operator= (TPointer<rconst_type const>      && PTR) { this->data = PTR; return this->data; }
    };
    
    
    
    template <function F>
    class TPointer<F> : public TValue<F*>
    {
    private:
        using TBaseClass = TValue<F*>;
    public:
        using TParam = F;
    
        TPointer(F* ptr) : TBaseClass(ptr) {}
    };
    
    
    
    template <typename T> TPointer(T*) -> TPointer<T>;
    
    template <function F> TPointer(F ) -> TPointer<F>;
    template <function F> TPointer(F*) -> TPointer<F>;
    
    
    
    template <typename T>
    struct std::remove_pointer<TPointer<T>>
    {
        using type = T;
    };
    
    template <typename T>
    struct std::remove_pointer<TPointer<T> const>
    {
        using type = T;
    };
    
    template <typename T>
    struct std::remove_pointer<TPointer<T> volatile>
    {
        using type = T;
    };
    
    template <typename T>
    struct std::remove_pointer<TPointer<T> const volatile>
    {
        using type = T;
    };
    */
    
    // Array ------------------------------------------------------------------------------
    
    template <typename T, size_t N>
    class TArray : public std::array<T, N>
    {
    public:
        using TBaseParam = T[N];
        using TBaseClass = std::array<T, N>;
        using TData = T[N];
    private:
        using TNegConst = std::conditional_t<std::is_const_v<T>, std::remove_const_t<T>, T const>;
    
        template <size_t... idx_> requires (sizeof...(idx_) == N)
        TArray(T const* ptr, std::index_sequence<idx_...>) : TBaseClass{ ptr[idx_]... } {}
    public:
        TArray() : TBaseClass{} {}
    
        TArray(TArray<TNegConst, N> const & other) : TArray(&other[0], std::make_index_sequence<N>{}) {}
    
        TArray(T const (&arr)[N]) : TArray(&arr[0], std::make_index_sequence<N>{}) {}
    
        TArray(std::array<T, N> const & arr) : TArray(&arr[0], std::make_index_sequence<N>{}) {}
    
        template <typename... Args> requires (sizeof...(Args) == N)
        TArray(Args&& ... args) : TBaseClass{ std::forward<Args>(args)... } {}
    
        operator TData const & () const { return *reinterpret_cast<TData const *>(this->data()); }
        operator TData       & ()       { return *reinterpret_cast<TData       *>(this->data()); }
    };
    
    //template <typename... Ts> requires (sizeof...(Ts) > 1)
    //TArray(Ts& ...) -> TArray<std::common_type_t<Ts...>, sizeof...(Ts)>;
    
    template <typename... Ts> requires (sizeof...(Ts) > 1)
    TArray(Ts const & ...) -> TArray<std::common_type_t<Ts...>, sizeof...(Ts)>;
    
    //template <typename... Ts> requires (sizeof...(Ts) > 1)
    //TArray(Ts&& ...) -> TArray<std::common_type_t<Ts...>, sizeof...(Ts)>;
    
    //template <typename T, size_t N>
    //TArray(T(&)[N]) -> TArray<T, N>;
    
    //template <typename T, size_t N>
    //TArray(T const (&)[N]) -> TArray<T const, N>;
    
    
    
    //template <typename T, size_t N>
    //class TValue<T[N]> : public TArray<T, N>
    //{
    //public:
    //    using TBaseParam = T[N];
    //    using TBaseClass = TArray<T, N>;
    //
    //    TValue(                ) : TBaseClass(              ) {}
    //    TValue(TBaseParam& obj ) : TBaseClass(          obj ) {}
    //    TValue(TBaseParam&& obj) : TBaseClass(std::move(obj)) {}
    //
    //    template <typename... Args>
    //    TValue(Args... args) : TBaseClass(std::forward<Args>(args)...) {}
    //};


    // Сортировка типов ----------------------------------------------------------------

    template <typename... Ts> struct TTypeList;

    template <typename...> struct TConcatTypeList;

    template <typename... Ts, typename... Us>
    struct TConcatTypeList<TTypeList<Ts...>, TTypeList<Us...>>
    {
        using type = TTypeList<Ts..., Us...>;
    };

    template <typename... Ts>
    using TConcatTypeList_t = typename TConcatTypeList<Ts...>::type;

    template <typename Left, typename Right>
    static constexpr bool type_less = sib::type_name<Left>() < sib::type_name<Right>();

    template <template <typename...> typename Template, typename... Types>
    class TSortTypeListAndInstantiate
    {
    private:
        template <template <typename...> typename Templ, typename...> struct instantiate;

        template <template <typename...> typename Templ, typename... Ts>
        struct instantiate<Templ, TTypeList<Ts...>>
        {
            using type = Templ<Ts...>;
        };

        template <template <typename...> typename Templ, typename... Ts>
        using instantiate_t = typename instantiate<Templ, Ts...>::type;

        template <int Count, typename... Ts>
        struct take;

        template <int Count, typename... Ts>
        using take_t = typename take<Count, Ts...>::type;

        template <typename... Ts>
        struct take<0, TTypeList<Ts...>>
        {
            using type = TTypeList<>;
            using rest = TTypeList<Ts...>;
        };

        template <typename A, typename... Ts>
        struct take<1, TTypeList<A, Ts...>>
        {
            using type = TTypeList<A>;
            using rest = TTypeList<Ts...>;
        };

        template <int Count, typename A, typename... Ts>
        struct take<Count, TTypeList<A, Ts...>>
        {
            using type = TConcatTypeList_t<TTypeList<A>, take_t<Count - 1, TTypeList<Ts...>>>;
            using rest = typename take<Count - 1, TTypeList<Ts...>>::rest;
        };

        template <typename...> struct sort_list;

        template <typename... Ts>
        using sorted_list_t = typename sort_list<Ts...>::type;

        template <typename A>
        struct sort_list<TTypeList<A>>
        {
            using type = TTypeList<A>;
        };

        template <typename A, typename B>
        struct sort_list<TTypeList<A, B>>
        {
            using type = std::conditional_t<type_less<A, B>, TTypeList<A, B>, TTypeList<B, A>>;
        };

        template <typename...> struct merge;

        template <typename... Ts>
        using merge_t = typename merge<Ts...>::type;

        template <typename... Bs>
        struct merge<TTypeList<>, TTypeList<Bs...>>
        {
            using type = TTypeList<Bs...>;
        };

        template <typename... As>
        struct merge<TTypeList<As...>, TTypeList<>>
        {
            using type = TTypeList<As...>;
        };

        template <typename AHead, typename... As, typename BHead, typename... Bs>
        struct merge<TTypeList<AHead, As...>, TTypeList<BHead, Bs...>>
        {
            using type = std::conditional_t<
                type_less<AHead, BHead>,
                TConcatTypeList_t<TTypeList<AHead>, merge_t<TTypeList<As...>, TTypeList<BHead, Bs...>>>,
                TConcatTypeList_t<TTypeList<BHead>, merge_t<TTypeList<AHead, As...>, TTypeList<Bs...>>>
            >;
        };

        template <typename... Ts>
        struct sort_list<TTypeList<Ts...>>
        {
            static constexpr auto first_size = sizeof...(Ts) / 2;
            using split = take<first_size, TTypeList<Ts...>>;
            using type = merge_t<
                sorted_list_t<typename split::type>,
                sorted_list_t<typename split::rest>
            >;
        };

    public:
        using result_type = instantiate_t<Template, sorted_list_t<TTypeList<Types...>>>;
    };

    template <typename T, typename... Ts>
    struct First
    {
        using type = T;
    };

    template <typename... Ts>
    using First_t = typename First<Ts...>::type;

    template <typename T, typename... Ts>
    constexpr bool is_sorted_v = type_less<T, First_t<Ts...>> and is_sorted_v<Ts...>;

    template <typename T>
    constexpr bool is_sorted_v<T> = true;



    // Кортеж уникальных отсортированных типов -----------------------------------------
    template <typename... Ts>
    class TUniqueTuple : public std::tuple<Ts...>
    {
        static_assert
            (
                sib::is_unique_v<Ts...>,
                "sib::unique_tuple: Template types must be unique and not not be implicitly convertible."
                );

        static_assert
            (
                sib::is_sorted_v<Ts...>,
                "sib::unique_tuple: Template types must be sorted."
                );
    };

    template <typename... Ts>
    using MakeUniqueTuple = typename TSortTypeListAndInstantiate<TUniqueTuple, Ts...>::result_type;


} // namespace sib
