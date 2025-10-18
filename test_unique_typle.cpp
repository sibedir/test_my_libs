#include "test_unique_typle.h"

#include "sib_type_traits.h"
#include "sib_unit_test.h"
#include "sib_unique_tuple.h"
#include "sib_wrapper.h"

// ---------------------------------------------------------------------------------------------------------------------

#include <set>

// ---------------------------------------------------------------------------------------------------------------------

enum TEnum { e_1 = 1, e_2, e_3, e_4, e_5 };

enum class TEnumClass { e_1 = 1, e_2, e_3, e_4, e_5 };

enum class TEnumClass123 : unsigned char { _1 = 1, _2, _3 };

// ---------------------------------------------------------------------------------------------------------------------

struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

// ---------------------------------------------------------------------------------------------------------------------

using namespace std::string_literals;

// ---------------------------------------------------------------------------------------------------------------------

static int foo(float f) { return static_cast<int>(f); }
static int bar(float f) { return static_cast<int>(-f); }

auto baz(int         const &) { return "do baz(int)"s;          }
auto baz(double      const &) { return "do baz(double)"s;       }
auto baz(std::string const &) { return "do baz(std::string&)"s; }

// ---------------------------------------------------------------------------------------------------------------------

template <std::size_t... idx_>
consteval auto gen_TP_impl(std::index_sequence<idx_...>)
{
    return sib::types_pack< sib::int_tag<idx_> ... > {};
}

template <size_t... idx_>
consteval auto gen_TL_impl(std::index_sequence<idx_...>)
{
    return sib::types_list< sib::int_tag<idx_> ... > {};
}

template <size_t N> using gen_TP = decltype(gen_TP_impl(std::make_index_sequence<N>{}));
template <size_t N> using gen_TL = decltype(gen_TL_impl(std::make_index_sequence<N>{}));

// ---------------------------------------------------------------------------------------------------------------------

template <typename T> struct TS_to_Str_Helper;

template <template <typename...> typename Tmpl, typename... Ts>
struct TS_to_Str_Helper<Tmpl<Ts...>>
{
    operator std::string() const
    {
        if constexpr (sizeof...(Ts) == 0) {
            return "<>";
        } else if constexpr (sizeof...(Ts) == 1) {
            return "<" + std::to_string(sib::types_first_t<Ts...>::value) + ">";
        } else {
            std::string res = "<";
            ((res += std::to_string(Ts::value) + ", "), ...);
            res[res.size() - 2] = '>';
            return res;
        }
    }
};

template <typename T>
std::string Types_to_Str() { return TS_to_Str_Helper<T>(); }

// ---------------------------------------------------------------------------------------------------------------------

enum class TF1 { f0, f1, f2, f3, f4, f5 };
enum class TF2 { f0, f1, f2, f3, f4, f5 };
enum class TF3 { f0, f1, f2, f3, f4, f5 };
enum class TF4 { f0, f1, f2, f3, f4, f5 };
enum class TF5 { f0, f1, f2, f3, f4, f5 };
enum class TF6 { f0, f1, f2, f3, f4, f5 };

// ---------------------------------------------------------------------------------------------------------------------

#define _ ,

// ---------------------------------------------------------------------------------------------------------------------

DEF_TEST(test_TUniqueTuple)
{
    sib::debug::Init();

    MSG();                                                //
    MSG("****************************************************************************************************");
    MSG("                                            TUniqueTuple                                            ");
    MSG("****************************************************************************************************");
    MSG();

    {
        BEG;
        DEF(int, i, = 1);

        sib ::MakeUniqueTuple<TEnumClass, std ::set<int>> ut111(TEnumClass::e_5);

        DEF(sib::MakeUniqueTuple <TEnumClass _ std::set<int>>, ut1, (TEnumClass::e_5));
        PRN(static_cast<int>(ut1));
        PRN(ut1.as<int>());
        PRN(ut1 == TEnumClass::e_5);

        DEF(TEnumClass, e, = TEnumClass::e_2);
        DEF(sib::MakeUniqueTuple <TEnumClass& _ std::set<int>>, ut2, (e));
        PRN(static_cast<int>(ut2));
        PRN(ut2.as<int>());
        PRN(ut2 == TEnumClass::e_2);

        DEF(sib::MakeUniqueTuple <int _ std::string>, ut3, ("000"));
        PRN(ut3 == "000");

        DEF(std::string, s, = "111");
        DEF(sib::TLeftReference<std::string>, sr, = s);
        DEF(sib::MakeUniqueTuple <int _ std::string&>, ut4, (s));
        PRN(ut4 == "111");
        PRN(sr == "111");
        PRN(s == "111");
        END;
    } {
        BEG;
        PRN(sib::is_constructible_from_tooneof_v<float _ int _ std::set<int> _ std::string>);
        PRN(sib::is_constructible_from_to_v<float _ int>);
        PRN(sib::is_constructible_from_to_v<float _ std::set<int>>);
        PRN(sib::is_constructible_from_to_v<float _ std::string>);

        PRN(sib::make_unique_tuple(42 _ std::string("qwerty") _ std::set<int>{}));
        PRN(sib::make_unique_tuple(std::set<int>{} _ 42 _ std::string("qwerty")));
        PRN(sib::make_unique_tuple(std::string("qwerty") _ 42 _ std::set<int>{}));
        PRN(sib::make_unique_tuple(42 _ std::string("qwerty") _ std::set<int>{}).c_str());
        PAS(sib::make_unique_tuple(std::string("qwerty") _ 42 _ std::set<int>{}), int);
        PAS(sib::make_unique_tuple(std::string("qwerty") _ 42 _ std::set<int>{}), float);
        PAS(42, std::vector<int>);
        //PAS(sib::make_unique_tuple(std::string("qwerty") _ 42 _ std::set<int>{}), std::vector<int>);
        END;
    } {
        BEG;
        DEF(int, i, = 42);
        DEF(std::string, s, = "qwerty");
        DEF(auto, ut, = sib::make_unique_tuple(i _ s));
        PRN(i);
        PRN(s);
        PRN(ut);
        PAS(ut, int);
        PAS(ut, std::string);
        END;
        EXE(i = 0);
        EXE(s = "0");
        EXE(ut.get<int>() = 1);
        EXE(ut = "1");
        PRN(ut);
        PAS(ut, int);
        PAS(ut, std::string);
        ASS(i == 0);
        ASS(s == "0");
        ASS(ut == 1);
        ASS(ut == "1");
        END;
    } {
        BEG;
        DEF(int, i, = 42);
        DEF(std::string, s, = "qwerty");
        DEF(auto, ut, = sib::MakeUniqueTuple<int& _ std::string&>(i _ s));
        PRN(i);
        PRN(s);
        PRN(ut);
        PAS(ut, int);
        PAS(ut, std::string);
        END;
        EXE(ut.get<int&>() = 111);
        EXE(ut = "111");
        PRN(ut);
        PAS(ut, int);
        PAS(ut, std::string);
        ASS(i == 111);
        ASS(s == "111");
        
        //{ sib::TLeftReference<std::string> rrr = s; void(rrr == 111); }
        { sib::TLeftReference<int        > rrr = i; void(rrr == 111); }
        void(ut == 111);

        ASS(ut == 111);
        END;
    } {
        BEG;
        EXE(
            static_assert(std::is_same_v<
                sib::MakeUniqueTuple<std::vector<int>, bool, std::string>,
                sib::MakeUniqueTuple<std::string, std::vector<int, std::allocator<int>>, bool>
            >)
        );
        EXE(
            static_assert(std::is_same_v<
                sib::MakeUniqueTuple<A, B, C>,
                sib::MakeUniqueTuple<B, C, A>
            >)
        );

        DEF(sib::MakeUniqueTuple<A _ B _ C>, tmp1, );
        DEF(sib::MakeUniqueTuple<C _ B _ A>, tmp2, );
        DEF(sib::MakeUniqueTuple<B _ A _ C>, tmp3, );

        DEF(sib::MakeUniqueTuple<std::vector<int> _ bool _ std::string>, tmp4, );
        DEF(sib::MakeUniqueTuple<std::string _ std::vector<int _ std::allocator<int>> _ bool>, tmp5, );

        EXE(static_assert(std::is_same_v<decltype(tmp1), decltype(tmp2)>));
        EXE(static_assert(std::is_same_v<decltype(tmp2), decltype(tmp3)>));
        EXE(static_assert(std::is_same_v<decltype(tmp3), decltype(tmp1)>));

        EXE(static_assert(std::is_same_v<decltype(tmp4), decltype(tmp5)>));
        EXE(static_assert(std::is_same_v<decltype(tmp5), decltype(tmp4)>));

        PRN(tmp1);
        PRN(tmp2);
        PRN(tmp3);
        PRN(tmp4);
        PRN(tmp5);

        PRN(sib::is_container_v<decltype(tmp4)>);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<int>, tmp, );
                    
        static_assert(std::is_assignable_v<int&, int>);

        EXE(tmp = 111);
        PRN(tmp);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<std::string>, tmp, );
        EXE(tmp = "xxxxxxxxx");
        PRN(sib::is_container_v<decltype(tmp)>);
        PRN(tmp);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<int _ std::string>, tmp, );
        EXE(tmp = 111);
        EXE(tmp = "xxxxxxxxx");
        PRN(sib::is_container_v<decltype(tmp)>);
        PRN(tmp);
        END;
    } {
    
        //static_assert(sib::is_any_of_v<Ts...> _T>

        BEG;
        DEF(sib::MakeUniqueTuple<int _ std::string _ std::vector<int>>, tmp, );
        EXE(tmp.get<std::vector<int>>() = std::vector<int>{1 _ 2 _ 3});
        EXE(tmp.get<int>() = 2.f);
        EXE(tmp = "xxxxxxxxx");
        PRN(sib::is_container_v<decltype(tmp)>);
        PRN(tmp.get<std::vector<int>>());
        PRN(tmp.get<int>());
        PRN(tmp.get<std::string>());
        PRN(tmp);
        EXE(tmp.get<int>() = 3.f);
        PRN(tmp.get<int>());
        END;

        DEF(float, f, = tmp);
        PRN(f);
        END;
    } {
        BEG;
        
        static_assert(sib::is_constructible_from_tooneof_v<const char [7], std::string>);
        static_assert(sib::no_duplicates_v<sib::construct_from_tooneof_select<const char [7], std::string>>);
        DEF(sib::MakeUniqueTuple<std::string>, ut, ("qwerty"));
        PRN(ut);
        END;
    } {
        BEG;
        DEF(sib::MakeUniqueTuple<int _ std::string>, ut, { "qwerty" _ 111 });
        DEF(std::string, s, = ut);
        PRN(ut);
        PRN(foo(ut));
        PRN(bar(ut));
        PRN(baz(ut));
        END;
    } {
        BEG;
        //DEF(auto, ut, = sib::MakeUniqueTuple<std::wstring _ int>());
        DEF(auto, ut, = sib::make_unique_tuple(L"qwerty"s _ 111));
        DEF(float, f, = ut);
        DEF(std::wstring, s, = ut);
        PRN(f);
        PRN(s);
        PRN(ut);
        PRN(foo(ut));
        PRN(bar(ut));
        PRN(baz(ut));
        END;
    } {
        BEG;
        EXE(using UT1 = sib::MakeUniqueTuple<gen_TP<10>>);
        EXE([[maybe_unused]] UT1 ut1{});
        MSG("    length     = ", sib::static_type_name<UT1>().size());
        MSG("    type count = ", sib::types_info<UT1::types<sib::types_pack>>::count);
        PRN(Types_to_Str<UT1>());
        END;

        EXE(using TL = sib::types_tail_t<10, gen_TL<20>>);
        EXE(using UT2 = sib::MakeUniqueTuple<TL>);
        EXE([[maybe_unused]] UT2 ut2{});
        MSG("    length     = ", sib::static_type_name<UT2>().size());
        MSG("    type count = ", sib::types_info<UT2::types<sib::types_list>>::count);
        PRN(Types_to_Str<UT2>());
        END;

        PRN(std::is_same_v<UT1 _ UT2>);
        END;
    } {
        BEG;
        DEF(auto, ut, = sib::MakeUniqueTuple<int _ std::string _ B>(1, "sfsdfsd"));
        EXE(ut = "qwerty");
        PRN(ut);
        PAS(ut, float);
        PAS(ut, std::string);
        PAS(ut, B);
        END;
    } {
        BEG;
        DEF(sib::TArray<int _ 4>, arr, { 1 _ 2 _ 3 _ 4 });
        DEF(auto, ut, = sib::MakeUniqueTuple<int(&)[4] _ C _ B>(arr));
        PRN(arr);
        PRN(decltype(ut)::types<>());
        PRN(decltype(ut)::veritable_types<>());
        PRN(ut);
        PAS(ut, B);
        PAS(ut, C);
        PAS(ut, int(&)[4]);
        END;
    } {
        BEG;
        DEF(int, i, = 0);
        DEF(std::string, s, = "");
        DEF(sib::MakeUniqueTuple<int _ std::string>, ut, { i _ s });
        EXE(ut = 42);
        EXE(ut = "qwerty");
        ASS(ut == 42);
        ASS(ut == "qwerty");
        PAS(ut, int);
        PAS(ut, std::string);
        PRN(i);
        PRN(s);
        PRN(ut.get<int>());
        PRN(ut.as<int>());
        PRN(ut.as<int&>());
        PRN(ut.as<int32_t&>());
        PRN(ut.as<float>());
        EXE(ut.as<int32_t&>() = 777);
        PAS(ut, int);
        END;
    } {
        BEG;
        DEF(int, i, = 42);
        DEF(std::string, s, = "qwerty");
        DEF(sib::MakeUniqueTuple<int _ std::string> const, ut, { i _ s });
        ASS(ut == 42);
        ASS(ut == "qwerty");
        PAS(ut, int);
        PAS(ut, std::string);
        PRN(ut.get<int>());
        PRN(ut.as<int>());
        PRN(ut.as<int const &>());
        PRN(ut.as<int32_t const &>());
        PRN(ut.as<float>());
        END;
    } {
        BEG;
        DEF(int, i, = 0);
        DEF(std::string, s, = "");
        DEF(sib::MakeUniqueTuple<int& _ std::string&>, ut, { i _ s });
        PRN(decltype(ut)::types<>());
        PRN(decltype(ut)::veritable_types<>());
        EXE(ut.as<int>() = 42);
        EXE(ut = "qwerty");
        ASS(i == 42);
        ASS(s == "qwerty");
        ASS(ut == 42);
        ASS(ut.as<std::string>() == "qwerty");
        PRN(i);
        PRN(s);
        PAS(ut, int);
        PAS(ut, std::string);
        PRN(ut.get<int&>());
        PRN(ut.as<int>());
        PRN(ut.as<int&>());
        PRN(ut.as<int32_t&>());
        PRN(ut.as<float>());
        EXE(ut.as<int32_t&>() = 777);
        PAS(ut, int);
        END;
    } {
        BEG;
        DEF(int, i, = 123);
        DEF(std::string, s, = "123");
        DEF(sib::MakeUniqueTuple<int const & _ std::string const &>, ut, { i _ s });
        PRN(decltype(ut)::types<>());
        PRN(decltype(ut)::veritable_types<>());
        EXE(i = 42);
        EXE(s = "qwerty");
        ASS(ut == 42);
        ASS(ut.as<std::string>() == "qwerty");
        PRN(i);
        PRN(s);
        PAS(ut, int);
        PAS(ut, std::string);
        PRN(ut.get<int const &>());
        PRN(ut.as<int>());
        PRN(ut.as<int const &>());
        PRN(ut.as<int32_t const &>());
        PRN(ut.as<float>());
        PAS(ut, int);
        END;
    } {
        BEG;
        TEnum e = e_1;
        DEF(sib::MakeUniqueTuple<std::string _ TEnum&>, ut, (e, ""));
        PRN(ut);
        PAS(ut, int);
        PAS(ut, std::string);
        PRN(e);
        PRN(ut.as<int>());
        PRN(ut.as<TEnum>() = e_4);
        ASS(e == e_4);
        PRN(ut.as<TEnum>());
        PRN(ut.get<TEnum&>() = e_2);
        ASS(e == e_2);
        PRN(ut.as<TEnum>());
        END;
    } {
        BEG;
        TEnumClass e = TEnumClass::e_1;
        TEnumClass& er = e;
        PAS(er, int);
        DEF(sib::MakeUniqueTuple<std::string _ TEnumClass&>, ut, (e, ""));
        PRN(ut);
        PAS(ut, TEnumClass);
        PAS(ut, std::string);
        PAS(e, int);
        PRN(decltype(ut)::veritable_types<>());
        PAS(ut, int);
        PRN(ut.as<TEnumClass>());
        PRN(ut.as<TEnumClass>() = TEnumClass::e_4);
        ASS(e == TEnumClass::e_4);
        PRN(ut.as<int>());
        PRN(ut.get<TEnumClass&>() = TEnumClass::e_2);
        ASS(e == TEnumClass::e_2);
        PAS(ut, int);
        END;
    } {
        BEG;
        DEF(TEnumClass123, e, = TEnumClass123::_1);
        DEF(TEnumClass123 &, er, = e);
        PRN(er);
        PAS(er, int);
        DEF(sib::MakeUniqueTuple<std::string _ TEnumClass123>, ut, (e, ""));
        PRN(static_cast<int>(ut));
        PRN(ut);
        PAS(ut, TEnumClass123);
        PAS(ut, std::string);
        PRN(ut.as<TEnumClass123>());
        PRN(ut.as<TEnumClass123>() = TEnumClass123::_3);
        ASS(e == TEnumClass123::_1);
        PRN(ut.as<TEnumClass123>());
        PRN(ut.get<TEnumClass123>() = TEnumClass123::_2);
        ASS(e == TEnumClass123::_1);
        PRN(ut.as<TEnumClass123>());
        END;
    } {
        BEG;
        DEF(int, i, = 42);
        DEF(std::set<int>, s, = { 1 _ 2 _ 3 });
        #define TTT decltype(s)
        #define TTTs TTT&, float, int*
        DEF(sib::MakeUniqueTuple<TTTs>, ut, (&i, s));
        PRN(ut);
        //auto bbb = static_cast<bool>(ut);
        //PAS(ut, bool);
        END;
    } {
        BEG;
        DEF(std::set<int>, s, = { 1 _ 2 _ 3 });
        #define TTT decltype(s)
        DEF(sib::MakeUniqueTuple<TTT _ float>, ut, (s, 0));
        PRN(ut);
        PRN(static_cast<bool>(ut));
        PAS(ut, bool);
        PRN(ut.as<bool const &>());
        PRN(ut.as<bool>());
        END;
    } {
        BEG;
        DEF(int, i, = 1);
        DEF(bool const &, a, = static_cast<bool const&>(i));
        DEF(bool const &, b, = static_cast<bool>(i));
        PRN(a);
        PRN(b);
        PRN(&i);
        PRN(&a);
        PRN(&b);
        PRN(static_cast<bool&&>(i));
        PRN(static_cast<bool const &>(i));
        PRN(&static_cast<bool const &>(i));
        PRN(&static_cast<bool const &>(static_cast<bool &&>(i)));
        END;
    } {
        BEG;
        DEF(TF1, f1, = TF1::f1);
        DEF(TF2, f2, = TF2::f2);
        DEF(TF3, f3, = TF3::f3);
        DEF(TF4, f4, = TF4::f4);
        DEF(TF5, f5, = TF5::f5);
        DEF(TF6, f6, = TF6::f0);
        DEF(auto, ut1, = sib::make_unique_tuple(f1 _ f2 _ f3 _ f4 _ f5 _ f6 _ 42));

        DEF(decltype(ut1), ut2,);
        DEF(decltype(ut1), ut3,);

        static_assert(std::is_same_v<decltype(ut1), decltype(ut2)>);
        
        EIS(ut1.get<int> (), int&);
        EIS(ut1.get<TF2> (), TF2&);
        EIS(ut1.as<int>  (), int&);
        EIS(ut1.as<float>(), float);
        EIS(ut1.as<TF2>  (), TF2&);
        EXE(ut1 = TF1::f3);
        EXE(ut1 = 666);
        EXE(ut3 = ut1);
        EXE(ut2 = std::move(ut3));
        EXE(std::vector<int> vi);
        EXE(vi.push_back([](TF1 const& f) { return static_cast<int>(f); }(ut1)));
        EXE(vi.push_back([](TF2 const& f) { return static_cast<int>(f); }(ut1)));
        EXE(vi.push_back([](TF3 const& f) { return static_cast<int>(f); }(ut1)));
        EXE(vi.push_back([](TF4 const& f) { return static_cast<int>(f); }(ut1)));
        EXE(vi.push_back([](TF5 const& f) { return static_cast<int>(f); }(ut1)));
        EXE(vi.push_back([](TF6 const& f) { return static_cast<int>(f); }(ut1)));
        EXE(vi.push_back([](int const& f) { return static_cast<int>(f); }(ut1)));
        MSG("ut1");
        PRN(vi);

        EXE(vi.clear());
        EXE(vi.push_back([](TF1 const& f) { return static_cast<int>(f); }(ut2)));
        EXE(vi.push_back([](TF2 const& f) { return static_cast<int>(f); }(ut2)));
        EXE(vi.push_back([](TF3 const& f) { return static_cast<int>(f); }(ut2)));
        EXE(vi.push_back([](TF4 const& f) { return static_cast<int>(f); }(ut2)));
        EXE(vi.push_back([](TF5 const& f) { return static_cast<int>(f); }(ut2)));
        EXE(vi.push_back([](TF6 const& f) { return static_cast<int>(f); }(ut2)));
        EXE(vi.push_back([](int const& f) { return static_cast<int>(f); }(ut2)));
        MSG("ut2");
        PRN(vi);

        DEF(auto, lam1, = [](TF3 const& f) { return static_cast<int>(f); });
        DEF(auto, lam2, = [](int f) { return static_cast<int>(f); });
        PRN(lam1(ut2));
        PRN(lam2(ut2));
        PRN(ut1.as<int>());
        PRN(ut1.as<float>());

        MSG("\n\n");
        DEF(auto, ut4, = sib::make_unique_tuple(TF3::f0 _ TF4::f0 _ 777));
        EXE(vi.clear());
        EXE(vi.push_back([](TF3 const& f) { return static_cast<int>(f); }(ut4)));
        EXE(vi.push_back([](TF4 const& f) { return static_cast<int>(f); }(ut4)));
        EXE(vi.push_back([](int const& f) { return static_cast<int>(f); }(ut4)));
        MSG("ut4");
        PRN(vi);

        DEF(decltype(ut1), ut5, {ut4});
        END;
    }

    sib::debug::outstream << std::endl;
    return 0;
}
