#include "sib_support.h"

#include <random>

namespace sib {

    // ----------------------------------------------------------------------------------- rand

    ::std::random_device __rd;
    ::std::mt19937 __gen(__rd());
    ::std::uniform_real_distribution<double> __dis(0, 1);

    /* random in [0, 1) */ inline double rand() { return __dis(__gen); }
    /* random in [0, X) */ inline double rand(double X) { return rand() * X; }

}  // namespace sib
