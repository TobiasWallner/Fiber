#pragma once

#include <type_traits>
#include <cinttypes>
#include <bit>

namespace embed{


    template <typename Int, typename std::enable_if<std::is_integral<Int>::value, int>::type = 0>
    constexpr Int pow(const Int base, const unsigned int exponent){
        Int result = 1;
        for(int i = 0; i < exponent; ++i){result *= base;}
        return result;
    }


    float frexp(float value, int* exponent);

    double frexp(double value, int* exponent);

    inline bool is_nan(float value) {
        union { float f; uint32_t i; } u = { value };
        return ((u.i & 0x7F800000) == 0x7F800000) && ((u.i & 0x007FFFFF) != 0);
    }

    inline bool is_pinf(float value) {
        union { float f; uint32_t i; } u = { value };
        return u.i == 0x7F800000;
    }

    inline bool is_ninf(float value) {
        union { float f; uint32_t i; } u = { value };
        return u.i == 0xFF800000;
    }

    inline bool is_inf(float value){
        return is_pinf(value) || is_ninf(value);
    }
    


}