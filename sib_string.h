#pragma once

#include <string>
#include "sib_type_traits.h"

namespace sib {

    template <::sib::Char Ch, typename Tr = ::std::char_traits<Ch>, typename Al = std::allocator<Ch>>
    class promiscuous_stringstream : public ::std::basic_stringstream<Ch, Tr, Al>
    {
    private:

        using base_type = ::std::basic_stringstream<Ch, Tr, Al>;
        using string_type = ::std::basic_string      <Ch, Tr, Al>;

    public:

        using char_type = Ch;
        using traits_type = Tr;
        using allocator_type = Al;
        using int_type = typename Tr::int_type;
        using pos_type = typename Tr::pos_type;
        using off_type = typename Tr::off_type;

        promiscuous_stringstream() = default;

        promiscuous_stringstream(base_type const& sstr) : base_type(sstr) {}
        promiscuous_stringstream(base_type&& sstr) : base_type(std::move(sstr)) {}

        template <typename Arg>
            requires(requires(base_type& bos, Arg arg) { bos << arg; })
        promiscuous_stringstream& operator<< (Arg&& arg) &
            noexcept(noexcept(static_cast<base_type&>(*this) << std::forward<Arg>(arg)))
        {
            static_cast<base_type&>(*this) << std::forward<Arg>(arg);
            return *this;
        }

        template <LikeString Str>
            requires(not requires(base_type& os, Str str) { os << str; }
        and is_castable_from_to_v<container_elem_t<Str>, char_type>)
            promiscuous_stringstream& operator<< (Str&& str)&
        {
            decltype(auto) bos = static_cast<base_type&>(*this);
            for (container_elem_t<Str> ch : str) bos << static_cast<char_type>(ch);
            return *this;
        }

    };

    template <Char Ch, typename Tr = ::std::char_traits<Ch>, typename Al = std::allocator<Ch>>
    class promiscuous_string : public ::std::basic_string<Ch, Tr, Al>
    {
    private:

        using base_type = ::std::basic_string<Ch, Tr, Al>;

        using support_bufer_type = promiscuous_stringstream<Ch, Tr, Al>;

        template <Char _Ch>
        using simple_string = ::std::basic_string<_Ch>;

        template <typename... Args>
        promiscuous_string init(Args&&... args)
        {
            support_bufer_type buf;
            (buf << ... << ::std::forward<Args>(args));
            return buf.str();
        }

    public:

        using value_type = Ch;
        using traits_type = Tr;
        using allocator_type = Al;

        using size_type = typename base_type::size_type;
        using difference_type = typename base_type::difference_type;
        using pointer = typename base_type::pointer;
        using const_pointer = typename base_type::const_pointer;
        using reference = typename base_type::reference;
        using const_reference = typename base_type::const_reference;
        using iterator = typename base_type::iterator;
        using const_iterator = typename base_type::const_iterator;
        using reverse_iterator = typename base_type::reverse_iterator;
        using const_reverse_iterator = typename base_type::const_reverse_iterator;

    public:

        //indiscriminate_string() = default;

        promiscuous_string(base_type const& str) : base_type(str) {}
        promiscuous_string(base_type&& str) : base_type(std::move(str)) {}

        template <::sib::LikeString Str>
        promiscuous_string(Str const& str) : base_type(str.begin(), str.end()) {}

        template <::sib::Char _Ch>
        promiscuous_string(_Ch const* ptr) : promiscuous_string(simple_string<_Ch>(ptr)) {}

        promiscuous_string(Ch ch) : base_type{ ch } {}

        template <typename... Args>
        promiscuous_string(Args&&... args) : base_type(init(std::forward<Args>(args) ...)) {}
    };


} // namespace sib
