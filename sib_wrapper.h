#pragma once

#include <utility>
#include <type_traits>
#include <array>
#include <initializer_list>

// Note: sib_wrapper library classes behave as if no volotile exists

namespace sib {

    struct TNullPtr;
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
    
    template <class   Class> requires (std::is_class_v  <Class>) struct TWrapperSpec<Class> { using type = Class; };
    
    template <typename Func> requires (std::is_function_v<Func>) struct TWrapperSpec<Func>  { using type = std::function<Func>; };
    
    template <typename T> struct TWrapperSpec<T&>  { using type = std::remove_reference_t<remove_all_wrapers_t<T>&> ; };
    template <typename T> struct TWrapperSpec<T&&> { using type = std::remove_reference_t<remove_all_wrapers_t<T>&&>; };
    
    

    template <typename T>
    using TWrapper = typename TWrapperSpec<T>::type;
    
    
    
    template <typename T>
    TWrapper<T> to_wrap(T&& source) { return TWrapper<T>(std::forward<T>(source)); }
    
    template <typename T, typename... Args>
    TWrapper<T> to_wrap(Args&&... args) { return TWrapper<T>( std::forward<Args>(args)... ); }
    
    
    
    // TNullPtr ---------------------------------------------------------------------------
    
    struct TNullPtr {
        using base_type = std::nullptr_t;
    
        TNullPtr(std::nullptr_t = nullptr) {}
        operator std::nullptr_t() const { return nullptr; }
        explicit operator void const * () const { return nullptr; }
        std::nullptr_t operator= (std::nullptr_t) { return nullptr; }
    };
    
    
    
    // Value ------------------------------------------------------------------------------
    
    template <typename T>
    class TValue {
    private:
        using ptr_base_type = std::remove_pointer_t<T>;
    public:
        using data_type = T;
        T data;
    
        constexpr TValue(     ) : data{   } {}
        constexpr TValue(T val) : data(val) {}
    
        template <typename AnyT>
        constexpr TValue(TValue<AnyT> other) : data(other.data) {}
    
        constexpr operator T const & () const { return data; }
        constexpr operator T       & ()       { return data; }

        constexpr T& operator= (T const& other) { return data = other; }

        constexpr ptr_base_type const * operator -> () const requires (std::is_pointer_v<T>) { return data; }
        constexpr ptr_base_type       * operator -> ()       requires (std::is_pointer_v<T>) { return data; }

        constexpr explicit operator bool () const requires (std::is_pointer_v<T>) { return data; }

        template <typename AnyT> requires (std::is_pointer_v<T>)
        constexpr explicit operator AnyT const * () const { return static_cast<AnyT const *>(data); }

        template <typename AnyT> requires (std::is_pointer_v<T>)
        constexpr explicit operator AnyT       * ()       { return static_cast<AnyT       *>(data); }

    };

    template <typename Ret, typename... Args>
    class TValue<Ret(*)(Args...)> {
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
    class TValue<E> {
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
        requires (
            (std::is_const_v<T>            )
         or 
            (    !std::is_arithmetic_v<T>
             and !std::is_enum_v<T>
             and !std::is_pointer_v<T>     )
        )
    class TValue<T> {
        static_assert(always_false<T>::value, "TValue can accept only mulable arithmetic types, enum or pointer.");
    public:
        template <typename... AnyT>
        constexpr TValue(AnyT... args) {}
    };
    
    template <typename T>
    TValue(T) -> TValue<remove_all_wrapers_t<std::remove_cvref_t<T>>>;
    

    
    // Pointer ----------------------------------------------------------------------------
    
    /*
    template <typename T>
    class TPointer {
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
    class TPointer<F> : public TValue<F*> {
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
    struct std::remove_pointer<TPointer<T>> {
        using type = T;
    };
    
    template <typename T>
    struct std::remove_pointer<TPointer<T> const> {
        using type = T;
    };
    
    template <typename T>
    struct std::remove_pointer<TPointer<T> volatile> {
        using type = T;
    };
    
    template <typename T>
    struct std::remove_pointer<TPointer<T> const volatile> {
        using type = T;
    };
    */
    
    // Array ------------------------------------------------------------------------------
    
    template <typename T, size_t N>
    class TArray : public std::array<T, N> {
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
    //class TValue<T[N]> : public TArray<T, N> {
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
    

} // namespace sib
