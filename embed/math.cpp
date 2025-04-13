#include "embed/math.hpp"

namespace embed{

    float frexp(float value, int* exponent){
        const uint32_t ivalue = *reinterpret_cast<uint32_t*>(&value);
        const int raw_exp = ((ivalue >> 23) & 0xFF);
        *exponent = raw_exp - 127;
        if (raw_exp == 0) return value; // subnormals / zero
        const uint32_t exp_mask = 0x7F800000;
        const uint32_t masked_value = (ivalue & ~exp_mask) | (127 << 23);
        return *reinterpret_cast<float*>(masked_value);
    }

    double frexp(double value, int* exponent){
        const uint64_t ivalue = *reinterpret_cast<uint64_t*>(&value);
        const int raw_exp = ((ivalue >> 52) & 0x7FF);
        *exponent = raw_exp - 1023;
        if (raw_exp == 0) return value; // subnormals / zero
        const uint64_t exp_mask = 0x7FF0000000000000ULL;
        const uint64_t masked_value = (ivalue & ~exp_mask) | (uint64_t(1023) << 52);
        return *reinterpret_cast<double*>(masked_value);
    }

}