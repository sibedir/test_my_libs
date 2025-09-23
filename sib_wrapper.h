#pragma once

#include <array>
#include <type_traits>
#include <utility>
#include <functional>

#include "sib_type_traits.h"

namespace sib {

    template <typename T> struct unwrap;
    template <typename T> using  unwrap_t = typename unwrap<T>::type;

    #define SIB_NOT_CV(T) not (std::is_const_v<T> or std::is_volatile_v<T>)

    template <typename T> struct TWrapperSpec;
    
    template <typename T>
        requires (SIB_NOT_CV(T) or is_function_v<T>)
    using TWrapper = typename TWrapperSpec<unwrap_t<T>>::type;



    // TNullPtr ---------------------------------------------------------------------------
    
    struct TNullPtr
    {
        using base_type = std::nullptr_t;
    
        constexpr TNullPtr() = default;
        constexpr TNullPtr(TNullPtr const &) = default;
        constexpr TNullPtr(TNullPtr      &&) = default;
        constexpr TNullPtr& operator=(TNullPtr const &) = default;
        constexpr TNullPtr& operator=(TNullPtr      &&) = default;
        constexpr ~TNullPtr() = default;

        constexpr TNullPtr(std::nullptr_t) noexcept {}
        constexpr operator std::nullptr_t() const noexcept { return nullptr; }
//        constexpr operator std::nullptr_t const & () const noexcept { return data; }
//        constexpr operator std::nullptr_t       & ()       noexcept { return data; }
        explicit constexpr operator void const * () const noexcept { return nullptr; }
        constexpr std::nullptr_t operator= (std::nullptr_t) noexcept { return nullptr; }
    };
    
    
    
    // TValue -----------------------------------------------------------------------------
    
    // sib::TValue accepts only non-const and non-volatile arithmetic types or enum.

    template <typename> class TValue;

    template <Arithmetic T> requires (SIB_NOT_CV(T))
    class TValue<T>
    {
    public:
        using base_type = T;
    private:
        T data;
    public:
        constexpr TValue() = default;
        constexpr TValue(TValue const &) = default;
        constexpr TValue(TValue      &&) = default;
        constexpr TValue& operator=(TValue const &) = delete; //default;
        constexpr TValue& operator=(TValue      &&) = delete; //default;
        constexpr ~TValue() = default;

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

        constexpr TValue(T const val) noexcept : data(val) {}
    
        constexpr operator T const & () const noexcept { return data; }
        constexpr operator T       & ()       noexcept { return data; }

        //constexpr T& operator = (T const & other) noexcept { return data = other; }

        template <AssignableFromTo<T&> AnyT>
        constexpr T& operator = (AnyT const & other) noexcept { return data = other; }

        // template <typename _T>
        // constexpr decltype(std::declval<T>() == std::declval<_T>()) operator == (_T const & other) const
        //     noexcept(noexcept(std::declval<T>() == std::declval<_T>()))
        // { return data == other; }
    };



    template <Enum E> requires (SIB_NOT_CV(E))
    class TValue<E>
    {
    public:
        using data_type = E;
        using underlying_type = std::underlying_type_t<E>;
    private:
        E data;
    public:
        constexpr TValue() = default;
        constexpr TValue(TValue const &) = default;
        constexpr TValue(TValue      &&) = default;
        constexpr TValue& operator=(TValue const &) = default;
        constexpr TValue& operator=(TValue      &&) = default;
        constexpr ~TValue() = default;

        constexpr TValue(E val) noexcept : data(val) {}

        constexpr explicit TValue(underlying_type val) noexcept requires (is_enum_class_v<E>) : data{ val } {}

        constexpr operator E const & () const noexcept { return data; }
        constexpr operator E       & ()       noexcept { return data; }
    
        template <std::integral Int>
        constexpr explicit operator Int () const noexcept { return static_cast<Int>(data); }
    
        constexpr E& operator= (E const &     other) noexcept { return data = other;               }
        constexpr E& operator= (underlying_type val) noexcept { return data = static_cast<E>(val); }

        template <typename _E>
        constexpr decltype(std::declval<E>() == std::declval<_E>()) operator == (_E const & other) const noexcept { return data == other; }
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
    TValue(T) -> TValue<unwrap_t<std::remove_cvref_t<T>>>;



    // TPointer ---------------------------------------------------------------------------

    template <typename T> class TPointer;

    template <typename T>
        requires (not std::is_reference_v<T> and not std::is_void_v<std::remove_cv_t<T>>)
    class TPointer<T>
    {
    public:
        using base_type = T;
        using data_type = T*;
    private:
        T* data;
    public:
        constexpr TPointer() = default;
        constexpr TPointer(TPointer const &) = default;
        constexpr TPointer(TPointer      &&) = default;
        constexpr TPointer& operator=(TPointer const &) = default;
        constexpr TPointer& operator=(TPointer      &&) = default;
        constexpr ~TPointer() = default;

        constexpr TPointer(T* ptr) noexcept : data(ptr) {}
    
        constexpr operator T* const & () const noexcept { return data; }
        constexpr operator T*       & ()       noexcept { return data; }

        template <typename _T>
        explicit constexpr operator _T* () const noexcept { return static_cast<_T*>(data); }

        constexpr operator bool () const noexcept { return data; }

        constexpr T*& operator = (T* const & other) noexcept { return data = other; }

        constexpr T&  operator * () const noexcept { return *data; }
        constexpr T*  operator ->() const noexcept { return  data; }

        constexpr T*& operator ++ () noexcept { return ++data; }
        constexpr T*& operator -- () noexcept { return --data; }

        constexpr T* operator ++ (int) noexcept { return data++; }
        constexpr T* operator -- (int) noexcept { return data--; }

        template <typename _P>
        constexpr decltype(std::declval<T*>() == std::declval<_P>()) operator == (_P const & other) const
            noexcept(noexcept(std::declval<T*>() == std::declval<_P>()))
        { return *this == other; }
    };

    template <typename T>
        requires (std::is_void_v<std::remove_cv_t<T>>)
    class TPointer<T>
    {
    private:
        T* data;
    public:
        using base_type = T;
        using data_type = T*;

        constexpr TPointer() = default;
        constexpr TPointer(TPointer const &) = default;
        constexpr TPointer(TPointer      &&) = default;
        constexpr TPointer& operator=(TPointer const &) = default;
        constexpr TPointer& operator=(TPointer      &&) = default;
        constexpr ~TPointer() = default;

        constexpr TPointer(T* const ptr) noexcept : data(ptr) {}
    
        constexpr operator T* const & () const noexcept { return data; }
        constexpr operator T*       & ()       noexcept { return data; }

        template <typename _T>
        explicit constexpr operator _T* () const noexcept { return static_cast<_T*>(data); }

        constexpr operator bool () const noexcept { return data; }

        constexpr T*& operator = (T* const & other) noexcept { return data = other; }

        constexpr T*  operator ->() const noexcept { return data; }

        constexpr T*& operator ++ () noexcept { return ++data; }
        constexpr T*& operator -- () noexcept { return --data; }

        constexpr T* operator ++ (int) noexcept { return data++; }
        constexpr T* operator -- (int) noexcept { return data--; }

        template <typename _P>
        constexpr decltype(std::declval<T*>() == std::declval<_P>()) operator == (_P const & other) const
            noexcept(noexcept(std::declval<T*>() == std::declval<_P>()))
        { return *this == other; }
    };

    template <typename Ret, typename... Args>
    class TPointer<Ret(Args...)>
    {
    public:
        using base_type = Ret(Args...);
        using data_type = base_type*;
    private:
        data_type data;
    public:
        constexpr TPointer() = default;
        constexpr TPointer(TPointer const &) = default;
        constexpr TPointer(TPointer      &&) = default;
        constexpr TPointer& operator=(TPointer const &) = default;
        constexpr TPointer& operator=(TPointer      &&) = default;
        constexpr ~TPointer() = default;

        constexpr TPointer(base_type ptr) noexcept : data(ptr) {}

        constexpr operator data_type () const noexcept { return data; }

        constexpr data_type& operator = (data_type ptr) noexcept { return data = ptr; }

        constexpr base_type& operator * () const noexcept { return *data; }

        Ret operator()(Args... args) const { return data(std::forward<Args>(args)...); }
    };

    template <typename T>
    TPointer(T*) -> TPointer<T>;

    // template <typename Ret, typename... Args>
    // TPointer(Ret(Args...)) -> TPointer<Ret(Args...)>;



    // Reference --------------------------------------------------------------------------


    template <typename T>
    class TLeftReference : public std::reference_wrapper<T>
    {
    private:
        using base = std::reference_wrapper<T>;
    public:
        using type = base::type;

        constexpr TLeftReference(T& arg)
            noexcept(noexcept(base(std::declval<T&>())))
            : base(arg)
        {}

        constexpr operator type& () const noexcept { return base::get(); }

        template <typename AnyT>
            requires(requires{ static_cast<AnyT>(std::declval<T>()); })
        explicit constexpr operator AnyT () const noexcept { return static_cast<AnyT>(base::get()); }

        constexpr T& operator= (T const & val) const noexcept { return base::get() = val; }

        template <typename U> requires( has_equal_v        <T, U> ) constexpr auto operator == (U const & other) const noexcept { return base::get() == other; }
        template <typename U> requires( has_not_equal_v    <T, U> ) constexpr auto operator != (U const & other) const noexcept { return base::get() != other; }
        template <typename U> requires( has_less_v         <T, U> ) constexpr auto operator <  (U const & other) const noexcept { return base::get() <  other; }
        template <typename U> requires( has_greater_v      <T, U> ) constexpr auto operator >  (U const & other) const noexcept { return base::get() >  other; }
        template <typename U> requires( has_less_equal_v   <T, U> ) constexpr auto operator <= (U const & other) const noexcept { return base::get() <= other; }
        template <typename U> requires( has_greater_equal_v<T, U> ) constexpr auto operator >= (U const & other) const noexcept { return base::get() >= other; }
    };

    // template <typename T>
    // class TReference
    // {
    // public:
    //     using value_type = std::remove_reference_t<T>;
    //     using data_type = value_type&;
    // private:
    //     data_type data;
    // public:
    //     constexpr TReference(T& ref) noexcept : data(ref) {}

    //     constexpr operator value_type const & () const noexcept { return data; }
    //     constexpr operator value_type       & ()       noexcept { return data; }

    //     template <sib::NotAnyOf<value_type, value_type const, value_type &, value_type const &> AnyT>
    //         requires(requires{ static_cast<AnyT>(data); })
    //     explicit constexpr operator AnyT () noexcept { return static_cast<AnyT>(data); }

    //     constexpr T& operator= (T const& val) noexcept { return data = val; }

    //     // template <typename _T>
    //     // constexpr decltype(std::declval<T>() == std::declval<_T>()) operator == (_T const & other) const
    //     //     noexcept(noexcept(std::declval<T>() == std::declval<_T>()))
    //     // { return data == other; }

    //     // template <typename _T>
    //     // constexpr decltype(std::declval<T>() == std::declval<_T>()) operator == (_T const & other) const noexcept { return data == other; }

    //     constexpr auto operator == (T const & other) const noexcept { return data == other; }
    //     constexpr auto operator != (T const & other) const noexcept { return data != other; }
    //     constexpr auto operator <  (T const & other) const noexcept { return data <  other; }
    //     constexpr auto operator >  (T const & other) const noexcept { return data >  other; }
    //     constexpr auto operator <= (T const & other) const noexcept { return data <= other; }
    //     constexpr auto operator >= (T const & other) const noexcept { return data >= other; }
    // };


    
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
    
    template <typename T, size_t N>
    TArray(std::array<T, N> const &) -> TArray<T, N>;

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



    // TClass -----------------------------------------------------------------------------

    template <Class C>
    class TClass : public C
    {
    public:
        constexpr TClass() = default;
        constexpr TClass(TClass const &) = default;
        constexpr TClass(TClass      &&) = default;
        constexpr TClass& operator=(TClass const &) = default;
        constexpr TClass& operator=(TClass      &&) = default;
        constexpr ~TClass() = default;

        constexpr TClass (C const & arg) : C{          arg } {};
        constexpr TClass (C      && arg) : C{std::move(arg)} {};
    
        template <typename _T>
            requires(is_constructible_from_to_v<_T, C>)
        TClass(_T&& other)
            noexcept(noexcept(C{std::forward<_T>(other)}))
            : C{ std::forward<_T>(other) }
        {}
    
        template <typename _C>
        constexpr decltype(std::declval<C>() == std::declval<_C>()) operator == (_C const & other) const
            noexcept(noexcept(std::declval<C>() == std::declval<_C>()))
        { return static_cast<C const &>(*this) == other; }
    };



    // TWrapper ---------------------------------------------------------------------------

    template <typename T          > struct unwrap                    { using type = T              ; };
    template <                    > struct unwrap<TNullPtr         > { using type = std::nullptr_t ; };
    template <typename T          > struct unwrap<TValue        <T>> { using type = unwrap_t<T>    ; };
    template <typename T          > struct unwrap<TPointer      <T>> { using type = unwrap_t<T>*   ; };
    template <typename T          > struct unwrap<TLeftReference<T>> { using type = unwrap_t<T>&   ; };
    template <typename T, size_t N> struct unwrap<TArray     <T, N>> { using type = unwrap_t<T> [N]; };
    template <typename T          > struct unwrap<TClass        <T>> { using type = unwrap_t<T>    ; };
    

    
    // исправить логику получения const параметров
    template <typename T>           struct TWrapperSpec                 { using type = TValue         <unwrap_t<T>>   ; };
    template <>                     struct TWrapperSpec<std::nullptr_t> { using type = TNullPtr                       ; };
    template <typename T>           struct TWrapperSpec<T&>             { using type = TLeftReference <unwrap_t<T>>   ; };
    template <typename T>           struct TWrapperSpec<T*>             { using type = TPointer       <unwrap_t<T>>   ; };
    template <typename T, size_t N> struct TWrapperSpec<T[N]>           { using type = TArray         <unwrap_t<T>, N>; };
    template <Class    C>           struct TWrapperSpec<C>              { using type = C                              ; };
    template <Function F>           struct TWrapperSpec<F>              { using type = std::function           <F>    ; };
    
    //template <typename T> struct TWrapperSpec<T&>  { using type = std::remove_reference_t<remove_all_wrapers_t<T>&> ; };
    //template <typename T> struct TWrapperSpec<T&&> { using type = std::remove_reference_t<remove_all_wrapers_t<T>&&>; };



    template <typename T>
    TWrapper<std::remove_cvref_t<T>> to_wrap(T&& source)
    {
        return std::forward<T>(source);
    }
    
    template <typename T, typename... Args>
    TWrapper<std::remove_reference_t<T>> to_wrap(Args&&... args)
    {
        return TWrapper<std::remove_reference_t<T>>(std::forward<Args>(args)... );
    }

} // namespace sib
