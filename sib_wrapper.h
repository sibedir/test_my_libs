#pragma once

#include <array>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <functional>

#include "sib_support.h"

namespace sib {

    struct TNullPtr;
    template <typename> class TConst;
    template <typename> class TValue;
    template <typename> class TReference;
    template <typename T> using TPointer = TValue<T*>;
    template <typename, size_t> class TArray;

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

    template <typename T> struct TWrapperSpec<T&> { using type = TReference<remove_all_wrapers_t<T>>; };

    template <typename T> struct TWrapperSpec<T*      > { using type = TPointer<remove_all_wrapers_t<T>>      ; };
    template <typename T> struct TWrapperSpec<T* const> { using type = TPointer<remove_all_wrapers_t<T>> const; };
    
    template <typename T, size_t N> struct TWrapperSpec<T       [N]> { using type = TArray<remove_all_wrapers_t<T>, N>      ; };
    template <typename T, size_t N> struct TWrapperSpec<T const [N]> { using type = TArray<remove_all_wrapers_t<T>, N> const; };

    template <class Class> requires (std::is_class_v  <Class>) struct TWrapperSpec<Class      > { using type = Class      ; };
    template <class Class> requires (std::is_class_v  <Class>) struct TWrapperSpec<Class const> { using type = Class const; };

    template <LikeFunction Func> struct TWrapperSpec<Func> { using type = std::function<Func>; };
    
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
    
        TNullPtr(std::nullptr_t = nullptr) noexcept {}
        operator std::nullptr_t() const noexcept { return nullptr; }
        explicit operator void const * () const noexcept { return nullptr; }
        std::nullptr_t operator= (std::nullptr_t) noexcept { return nullptr; }
    };
    
    
    
    // Value ------------------------------------------------------------------------------
    
    template <typename T>
    class TValue
    {
        static_assert(always_false<T>::value, "sib::TValue accepts only non-const arithmetic types, enums, or pointers — and never volatile types.");
    };

    template <typename T>
        requires (
            not ( std::is_const_v<T>      or std::is_volatile_v<T> )
            and ( std::is_arithmetic_v<T> or std::is_pointer_v<T>  )
        )
    class TValue<T>
    {
    public:
        using data_type = T;
    private:
        using base_ptr_type = std::remove_pointer_t<T>;
        T data;
    public:
        constexpr TValue(     ) noexcept : data{   } {}
        constexpr TValue(T val) noexcept : data(val) {}
    
        template <typename... Args>
            requires (std::is_constructible_v<T, Args...>)
        constexpr TValue(Args&&... args)
            noexcept(noexcept(T(std::forward<Args>(args) ...)))
            : data(std::forward<Args>(args) ...)
        {}

        template <typename AnyT>
            requires (std::is_constructible_v<T, AnyT>)
        constexpr TValue(TWrapper<AnyT> other)
            noexcept(noexcept(T(std::declval<AnyT>())))
            : data(other)
        {}
        
        constexpr operator T const & () const noexcept { return data; }
        constexpr operator T       & ()       noexcept { return data; }

        constexpr T& operator= (T const & other) noexcept { return data = other; }

        constexpr T const operator -> () const noexcept requires (std::is_pointer_v<T>) { return data; }
        constexpr T       operator -> ()       noexcept requires (std::is_pointer_v<T>) { return data; }

        constexpr explicit operator bool () const noexcept requires (std::is_pointer_v<T>) { return data; }

        constexpr TValue& operator ++ () noexcept requires (std::is_pointer_v<T>) { ++data; return *this; }
        constexpr TValue& operator -- () noexcept requires (std::is_pointer_v<T>) { --data; return *this; }

        constexpr T operator ++ (int) noexcept requires (std::is_pointer_v<T>) { return data++; }
        constexpr T operator -- (int) noexcept requires (std::is_pointer_v<T>) { return data--; }

        template <typename AnyT> requires(is_pointer_v<AnyT>)
        constexpr explicit operator AnyT const () const noexcept { return static_cast<AnyT const>(data); }

        template <typename AnyT> requires(is_pointer_v<AnyT>)
        constexpr explicit operator AnyT       ()       noexcept { return static_cast<AnyT      >(data); }
    };

    template <typename Ret, typename... Args>
    class TValue<Ret(*)(Args...)>
    {
    public:
        using data_type = Ret(*)(Args...);
    private:
        data_type data;
    public:
        constexpr TValue(data_type ptr) noexcept : data(ptr) {}

        constexpr operator data_type () const noexcept { return data; }

        constexpr data_type& operator= (data_type ptr) noexcept { return data = ptr; }

        Ret operator()(Args... args) const { return data(std::forward<Args>(args)...); }
    };

    template <Enum E> requires (not std::is_const_v<E>)
    class TValue<E>
    {
    public:
        using data_type = E;
        using underlying_type = std::underlying_type_t<E>;
    private:
        E data;
    public:
        constexpr TValue(     ) noexcept : data{   } {}
        constexpr TValue(E val) noexcept : data(val) {}

        constexpr explicit TValue(underlying_type val) noexcept requires (is_enum_class_v<E>) : data{ val } {}

        constexpr operator E const & () const noexcept { return data; }
        constexpr operator E       & ()       noexcept { return data; }
    
        template <std::integral Int>
        constexpr explicit operator Int () const noexcept { return static_cast<Int>(data); }
    
        constexpr E& operator= (E const &     other) noexcept { return data = other;               }
        constexpr E& operator= (underlying_type val) noexcept { return data = static_cast<E>(val); }
    };


    
    //template <typename T>
    //constexpr bool is_like_enum_v = std::is_enum_v<T>;
    //
    //template <Enum E>
    //constexpr bool is_like_enum_v<TValue<E>> = true;
    //
    //template <typename T>
    //using is_like_enum = std::bool_constant<is_like_enum_v<T>>;
    //
    //template <typename T>
    //concept like_enum = is_like_enum_v<T>;
    //
    //template <typename T>
    //struct underlying_type : std::underlying_type<T> {};

    //template <Enum E>
    //struct underlying_type<TValue<E>> {
    //    using type = std::underlying_type_t<E>;
    //};

    //template <like_enum E>
    //using underlying_type_t = typename underlying_type<E>::type;

    template <typename T>
    TValue(T) -> TValue<remove_all_wrapers_t<std::remove_cvref_t<T>>>;
    


    // Reference --------------------------------------------------------------------------

    template <typename T>
    class TReference
    {
    public:
        using data_type = T&;
    private:
        T& data;
    public:
        constexpr TReference(T& ref) noexcept : data(ref) {}

        constexpr operator T const & () const noexcept { return data; }
        constexpr operator T       & ()       noexcept { return data; }

        //template <typename AnyT>
        constexpr T& operator= (T const & val) noexcept { return data = val; }

        //constexpr std::strong_ordering operator<=>(TReference const &) const = default;
        //constexpr std::strong_ordering operator<=>(T const& other) const noexcept { return data <=> other; }
    };


    
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
        TArray(T const* ptr, std::index_sequence<idx_...>) noexcept : TBaseClass{ ptr[idx_]... } {}
    public:
        TArray() noexcept : TBaseClass{} {}
    
        TArray(TArray<TNegConst, N> const & other) noexcept : TArray(&other[0], std::make_index_sequence<N>{}) {}
    
        TArray(T const (&arr)[N]) noexcept : TArray(&arr[0], std::make_index_sequence<N>{}) {}
    
        TArray(std::array<T, N> const & arr) noexcept : TArray(&arr[0], std::make_index_sequence<N>{}) {}
    
        template <typename... Args> requires (sizeof...(Args) == N)
        TArray(Args&& ... args) noexcept : TBaseClass{ std::forward<Args>(args)... } {}
    
        operator TData const & () const noexcept { return *reinterpret_cast<TData const *>(this->data()); }
        operator TData       & ()       noexcept { return *reinterpret_cast<TData       *>(this->data()); }
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

} // namespace sib
