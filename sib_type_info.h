#pragma once

#include <type_traits>

//#include <string_view>
#include <string>
#include <typeinfo>
#include <vector>
//#include <set>
//#include <map>

namespace sib {

    // ----------------------------------------------------------------------------------- run-time type name

    template <typename...> struct TTypeInfo;

    template <typename T>
    constexpr auto type_name() noexcept
    {
        return TTypeInfo<T>::full_name();
    }

    template <typename T>
    constexpr auto type_name(T&&) noexcept
    {
        return type_name<T>();
    }

    //template <auto V>
    //constexpr auto type_name() noexcept
    //{
    //    return type_name<decltype(V)>();
    //}

    template <typename T>
    struct TTypeInfo<T> {
    private:
        using type = T;
        using rr_type = ::std::remove_reference_t<T>;
    public:
        static constexpr auto is_const = ::std::is_const_v     <rr_type>;
        static constexpr auto is_volat = ::std::is_volatile_v  <rr_type>;
        static constexpr auto is_ref   = ::std::is_reference_v <   type>;
        static constexpr auto is_arr   = ::std::is_array_v     <rr_type>;

        static ::std::string full_name() noexcept
        {
            return ::std::string(
                _low_name() +
                _const_symbol() +
                _volat_symbol() +
                _ref_symbol() +
                _array_size_symbol<rr_type>::get()
            );
        }
    private:
        template <typename _T> requires(not ::std::is_void_v<_T>)
        TTypeInfo(_T const &) : TTypeInfo() {};

        TTypeInfo() {};

        static ::std::string _low_name() noexcept
        {
            #define _SIB_IF_ELSE_(type)                                     \
                if constexpr (std::is_same_v<::std::remove_cvref_t<T>, type>) \
                { return #type; }                                           \
                else                                                        \

            //if constexpr (is_instantiation_v<TUniqueTuple, T>) {
            //    return #type;
            //}
            //else

            _SIB_IF_ELSE_(std::vector<bool>)
            _SIB_IF_ELSE_(std::vector<char>)
            _SIB_IF_ELSE_(std::vector<short>)
            _SIB_IF_ELSE_(std::vector<int>)
            _SIB_IF_ELSE_(std::vector<long>)
            _SIB_IF_ELSE_(std::vector<long long>)
            _SIB_IF_ELSE_(std::vector<float>)
            _SIB_IF_ELSE_(std::vector<double>)
            _SIB_IF_ELSE_(std::string)
            _SIB_IF_ELSE_(std::wstring) {
                return typeid(std::remove_extent_t<::std::remove_cvref_t<T>>).name();
            }

            #undef _SIB_IF_ELSE_
        }

        static ::std::string _const_symbol() noexcept
        {
            if constexpr (is_const) { return " const"; }
            else { return ""; }
        }

        static ::std::string _volat_symbol() noexcept
        {
            if constexpr (is_volat) { return " volatile"; }
            else { return ""; }
        }

        static ::std::string _ref_symbol() noexcept
        {
            if constexpr (not is_ref) { return ""; }
            else
            {
                if constexpr (std::is_lvalue_reference_v<T>)
                {
                    if constexpr (is_arr) { return " (&)"; }
                    else { return " &"; }
                }
                else
                {
                    if constexpr (is_arr) { return " (&&)"; }
                    else { return " &&"; }
                }
            }
        };

        template <typename _T>
        struct _array_size_symbol { static ::std::string get() noexcept { return ""; } };

        template <typename _T>
        struct _array_size_symbol<_T[]> { static ::std::string get() noexcept { return " []"; } };

        template <typename _T, size_t _N>
        struct _array_size_symbol<_T[_N]> { static ::std::string get() noexcept { return " [" + ::std::to_string(_N) + "]"; } };
    };

} // namespace sib
