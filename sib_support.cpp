#include "sib_support.h"

#include <random>

namespace sib {

    // ----------------------------------------------------------------------------------- rand

    ::std::random_device __rd;
    ::std::mt19937 __gen(__rd());
    ::std::uniform_real_distribution<double> __dis(0, 1);

    /* random in [0, 1) */ inline double rand() { return __dis(__gen); }
    /* random in [0, X) */ inline double rand(double X) { return rand() * X; }



    // ----------------------------------------------------------------------------------- convertion

    inline std::wstring string_to_wstring(std::string const& str)
    {
        return std::wstring(str.begin(), str.end());
    }

    inline std::wstring string_to_wstring()
    {
        return std::wstring();
    }

    inline std::string wstring_to_string(std::wstring const& str)
    {
        return std::string(str.begin(), str.end());
    }

    inline std::string wstring_to_string()
    {
        return std::string();
    }


}  // namespace sib
