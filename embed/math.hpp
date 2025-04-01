#pragma once

#include <type_traits>
#include <cinttypes>
#include <bit>
#include <cmath>

namespace embed{


    template <typename Int, typename std::enable_if<std::is_integral<Int>::value, int>::type = 0>
    constexpr Int pow(const Int base, const unsigned int exponent){
        Int result = 1;
        for(int i = 0; i < exponent; ++i){result *= base;}
        return result;
    }


    constexpr float frexp(float value, int* exponent){
        const uint32_t ivalue = std::bit_cast<uint32_t, float>(value);
        const int raw_exp = ((ivalue >> 23) & 0xFF);
        *exponent = raw_exp - 127;
        if (raw_exp == 0) return value; // subnormals / zero
        const uint32_t exp_mask = 0x7F800000;
        const uint32_t masked_value = (ivalue & ~exp_mask) | (127 << 23);
        return std::bit_cast<float, uint32_t>(masked_value);
    }

    constexpr double frexp(double value, int* exponent){
        const uint64_t ivalue = std::bit_cast<uint64_t, double>(value);
        const int raw_exp = ((ivalue >> 52) & 0x7FF);
        *exponent = raw_exp - 1023;
        if (raw_exp == 0) return value; // subnormals / zero
        const uint64_t exp_mask = 0x7FF0000000000000ULL;
        const uint64_t masked_value = (ivalue & ~exp_mask) | (uint64_t(1023) << 52);
        return std::bit_cast<double, uint64_t>(masked_value);
    }

    inline long double frexp(const long double& value, int* exponent){
        return std::frexp(value, exponent);
    }

}