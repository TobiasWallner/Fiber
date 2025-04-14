

"""
const float log10_2 = float(0.30102999566f); // log_10(2)
const float log2_10 = float(3.32192809489f); // log_2(10)
const float exponent_b10_f = exponent * log10_2;
const int exponent_b10_i = static_cast<int>(exponent_b10_f);
const float q = exponent_b10_f - exponent_b10_i;
const float mantissa_b10 = mantissa * std::exp2(q * log2_10);
"""

import numpy as np
import matplotlib.pyplot as plt

exp2 = np.arange(-127, 128)
exp10f = exp2 * np.log10(2)
exp10i = exp10f.astype(np.int32)
error = exp10f - exp10i
correction = np.exp2(error * np.log2(10))

exp2 = np.arange(-127, -1)
exp10f = exp2 * np.log10(2)
exp10i = exp10f.astype(np.int32)
error = exp10f - exp10i
negative_correction = np.exp2(error * np.log2(10))

negative_correction_i16 = (negative_correction * 65536).astype(np.uint32)
print("negative correction")
for elem in negative_correction_i16:
    print(f"{elem}, ", end='')
print("")
print("")
print(f"scaling factor: {1/65536.0}")
print("")
print("")

exp2 = np.arange(0, 128)
exp10f = exp2 * np.log10(2)
exp10i = exp10f.astype(np.int32)
error = exp10f - exp10i
positive_correction = np.exp2(error * np.log2(10))

positive_correction_i16 = (positive_correction * 6553.6).astype(np.uint32)
print("positive correction")
for elem in positive_correction_i16:
    print(f"{elem}, ", end='')
    
print("")
print("")
print(f"scaling factor: {1/6553.6}")
print("")
print("")