#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include "sib_wrapper.h"
#include "sib_unique_tuple.h"

namespace sib {

    template <typename T, typename...Next>
    struct TTypeInfo {
        static std::string full_name()
        {
            return type_name<T>() + ", " + TTypeInfo<Next...>::full_name();
        }
    };

    template <typename T>
    struct TTypeInfo<T> {
    private:
        using rr_type = std::remove_reference_t<T>;
    public:
        TTypeInfo() {};
        TTypeInfo(T&) {};

        static constexpr auto is_const = std::is_const_v     <rr_type>;
        static constexpr auto is_volat = std::is_volatile_v  <rr_type>;
        static constexpr auto is_ref = std::is_reference_v <T      >;
        static constexpr auto is_arr = std::is_array_v     <rr_type>;

        static std::string _low_name()
        {
            #define _SIB_IF_ELSE_(type)                                     \
                if constexpr (std::is_same_v<std::remove_cvref_t<T>, type>) \
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
                _SIB_IF_ELSE_(std::wstring)
            {
                return typeid(std::remove_extent_t<std::remove_cvref_t<T>>).name();
            }

            #undef _SIB_IF_ELSE_
        }

        static std::string _const_symbol()
        {
            if constexpr (is_const) { return " const"; }
            else { return ""; }
        }

        static std::string _volat_symbol()
        {
            if constexpr (is_volat) { return " volatile"; }
            else { return ""; }
        }

        static std::string _ref_symbol()
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
        struct _array_size_symbol { static std::string get() { return ""; } };

        template <typename _T>
        struct _array_size_symbol<_T[]> { static std::string get() { return " []"; } };

        template <typename _T, size_t _N>
        struct _array_size_symbol<_T[_N]> { static std::string get() { return " [" + std::to_string(_N) + "]"; } };

    public:
        static std::string full_name()
        {
            return std::string(
                _low_name() +
                _const_symbol() +
                _volat_symbol() +
                _ref_symbol() +
                _array_size_symbol<rr_type>::get()
            );
        }
    };

    template <typename T>
    auto type_name() noexcept
    {
        return TTypeInfo<T>::full_name();
    }

    template <typename T>
    auto type_name(T&&) noexcept
    {
        return type_name<T>();
    }

} // namespace sib
