import matplotlib.pyplot as plt
import numpy as np

def decode_linear(code):
    return code  # direct mapping
    
def decode_spacing2(code):
    result = 0
    for i in range(15):
        result |= ((code >> i) & 1) << (i * 2)
    return result
    
def decode_spacing3(code):
    result = 0
    for i in range(15):
        result |= ((code >> i) & 1) << (i * 3)
    return result
    
def decode_spacing4(code):
    result = 0
    for i in range(15):
        result |= ((code >> i) & 1) << (i * 4)
    return result
    
def decode_exp_spacing(code):
    positions = [i * 2 for i in range(15)]  # e.g., spacing = 2
    result = 0
    for i, bitpos in enumerate(positions):
        result |= ((code >> i) & 1) << bitpos
    return result

def next_encodable_spacing_0(user_size):
    return user_size if user_size < (1 << 15) else (1 << 15)  # max = 32767
    
    
def next_encodable_spacing_1(user_size):
    result = 0b101010101010101010101010101010
    bit_flip_poses = [29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1]
    for i in range(15):
        # try to clear the largest pos-bit to make it smaller
        bit_flip_pos = bit_flip_poses[i]
        temp = result ^ (1 << bit_flip_pos)
        
        # if temp got smaller but is still larger than user_size --> take it
        if(temp > user_size):
            result = temp
    # result is now the smallest number that is larger than than user_size and still representable in the encoding.
    return result
    
def next_encodable_spacing_2(user_size):
    result = 0b10110110110110110110110
    bit_flip_poses = [22, 20, 19, 17, 16, 14, 13, 11, 10, 8, 7, 5, 4, 2, 1]
    for i in range(15):
        # try to clear the largest pos-bit to make it smaller
        bit_flip_pos = bit_flip_poses[i]
        temp = result ^ (1 << bit_flip_pos)
        
        # if temp got smaller but is still larger than user_size --> take it
        if(temp > user_size):
            result = temp

    # result is now the smallest number that is larger than than user_size and still representable in the encoding.
    return result
    
def next_encodable_spacing_3(user_size):
    result = 0b11101110111011101110
    bit_flip_poses = [19, 18, 17, 15, 14, 13, 11, 10, 9, 7, 6, 5, 3, 2, 1]
    for i in range(15):
        # try to clear the largest pos-bit to make it smaller
        bit_flip_pos = bit_flip_poses[i]
        temp = result ^ (1 << bit_flip_pos)
        
        # if temp got smaller but is still larger than user_size --> take it
        if(temp > user_size):
            result = temp

    # result is now the smallest number that is larger than than user_size and still representable in the encoding.
    return result
    
def next_encodable_spacing_4(user_size):
    result = 0b1110111101111011110
    bit_flip_poses = [18, 17, 16, 14, 13, 12, 11, 9, 8, 7, 6, 4, 3, 2, 1]
    for i in range(15):
        # try to clear the largest pos-bit to make it smaller
        bit_flip_pos = bit_flip_poses[i]
        temp = result ^ (1 << bit_flip_pos)
        
        # if temp got smaller but is still larger than user_size --> take it
        if(temp > user_size):
            result = temp

    # result is now the smallest number that is larger than than user_size and still representable in the encoding.
    return result
    
def next_encodable_spacing_exp(user_size):
    result = 0b11111011110111011010
    bit_flip_poses = [19, 18, 17, 16, 15, 13, 12, 11, 10, 8, 7, 6, 4, 3, 1]
    for i in range(15):
        # try to clear the largest pos-bit to make it smaller
        bit_flip_pos = bit_flip_poses[i]
        temp = result ^ (1 << bit_flip_pos)
        
        # if temp got smaller but is still larger than user_size --> take it
        if(temp >= user_size):
            result = temp

    # result is now the smallest number that is larger than than user_size and still representable in the encoding.
    return result

def next_encodable_variable_prefix(user_size):
    value = user_size

    bit_width = 15
    encoded_result = np.int64(0)
    value_result = np.int64(0)
    base = np.int64(1 << (bit_width-1))
    
    value_scaling = 1
    scaling_shift = 1
    value_scaling_amp = 2
    while(not(value < (base>>1)) and base != 0):
        # calculate encoded and representable value
        encoded_result = encoded_result | base
        base = base >> 1
        value_result += base * value_scaling
        
        # reduce input with already assigned storage + round up
        value = (value - base)
        round_up = (value & ((1<<scaling_shift)-1)) > 0
        value2 = value
        value = value // value_scaling_amp
        value2 = value2 >> scaling_shift
        if(value != value2):
            print(f"value_scaling_amp: {value_scaling_amp}, scaling_shift: {scaling_shift}")
            print(f"value: {value}, value2: {value2}")
            assert(value==value2)
        value = value + round_up
        value2 = value2 + round_up
        assert(value==value2)
        # prepare next iteration
        value_scaling2 = value_scaling
        value_scaling = value_scaling * value_scaling_amp
        value_scaling2 = value_scaling2 << scaling_shift
        assert(value_scaling2==value_scaling)
        
        value_scaling_amp = value_scaling_amp << 1
        scaling_shift = scaling_shift + 1
        assert(value_scaling_amp == (1<<scaling_shift))
        
    
    if(base == 0):
        value = 0
    
    encoded_result = encoded_result | value
    value_result = value_result + value * value_scaling
    
    return value_result    
        
        


requested_sizes = np.logspace(0, 9, 100000, dtype=np.int64);

allocated_sizes_spacing_0 = np.array([next_encodable_spacing_0(i) for i in requested_sizes], dtype=np.int64)
allocated_sizes_spacing_1 = np.array([next_encodable_spacing_1(i) for i in requested_sizes], dtype=np.int64)
allocated_sizes_spacing_2 = np.array([next_encodable_spacing_2(i) for i in requested_sizes], dtype=np.int64)
allocated_sizes_spacing_3 = np.array([next_encodable_spacing_3(i) for i in requested_sizes], dtype=np.int64)
allocated_sizes_spacing_4 = np.array([next_encodable_spacing_4(i) for i in requested_sizes], dtype=np.int64)
allocated_sizes_spacing_exp = np.array([next_encodable_spacing_exp(i) for i in requested_sizes], dtype=np.int64)
allocated_sizes_variable_prefix = np.array([next_encodable_variable_prefix(i) for i in requested_sizes], dtype=np.int64)


# Plot 1: Requested size vs allocated size
plt.figure()
plt.loglog(requested_sizes, requested_sizes, label="ideal")
plt.loglog(requested_sizes, allocated_sizes_spacing_0, label="spacing: 0")
plt.loglog(requested_sizes, allocated_sizes_spacing_1, label="spacing: 1")
plt.loglog(requested_sizes, allocated_sizes_spacing_2, label="spacing: 2")
plt.loglog(requested_sizes, allocated_sizes_spacing_3, label="spacing: 3")
plt.loglog(requested_sizes, allocated_sizes_spacing_4, label="spacing: 4")
plt.loglog(requested_sizes, allocated_sizes_spacing_exp, label="spacing: exp")
plt.loglog(requested_sizes, allocated_sizes_variable_prefix, label="var pref")
plt.title("Decoded Size vs Encoded Value")
plt.xlabel("Requested Memory")
plt.ylabel("Allocated Memory")
plt.legend();
plt.show(block=False)

# Plot 3: Waste
waste_0 = (allocated_sizes_spacing_0 - requested_sizes)
waste_0 = np.where(waste_0 > 0, waste_0, 0)

waste_1 = (allocated_sizes_spacing_1 - requested_sizes)
waste_1 = np.where(waste_1 > 0, waste_1, 0)

waste_2 = (allocated_sizes_spacing_2 - requested_sizes)
waste_2 = np.where(waste_2 > 0, waste_2, 0)

waste_3 = (allocated_sizes_spacing_3 - requested_sizes)
waste_3 = np.where(waste_3 > 0, waste_3, 0)

waste_4 = (allocated_sizes_spacing_4 - requested_sizes)
waste_4 = np.where(waste_4 > 0, waste_4, 0)

waste_exp = (allocated_sizes_spacing_exp - requested_sizes)
waste_exp = np.where(waste_exp > 0, waste_exp, 0)

waste_var_pref = (allocated_sizes_variable_prefix - requested_sizes)
waste_var_pref = np.where(waste_var_pref > 0, waste_var_pref, 0)

plt.figure()
plt.loglog(requested_sizes, waste_0, label="spacing: 0")
plt.loglog(requested_sizes, waste_1, label="spacing: 1")
plt.loglog(requested_sizes, waste_2, label="spacing: 2")
plt.loglog(requested_sizes, waste_3, label="spacing: 3")
plt.loglog(requested_sizes, waste_4, label="spacing: 4")
plt.loglog(requested_sizes, waste_exp, label="spacing: exp")
plt.loglog(requested_sizes, waste_var_pref, label="var pref")
plt.title("Waste")
plt.xlabel("Requested Memory")
plt.ylabel("Waste in byte")
plt.legend();
plt.show(block=False)

# Plot 3: Waste in %
rel_waste_0 = waste_0 / requested_sizes
rel_waste_1 = waste_1 / requested_sizes
rel_waste_2 = waste_2 / requested_sizes
rel_waste_3 = waste_3 / requested_sizes
rel_waste_4 = waste_4 / requested_sizes
rel_waste_exp = waste_exp / requested_sizes
rel_waste_var_pref = waste_var_pref / requested_sizes

plt.figure()
plt.semilogx(requested_sizes, rel_waste_0 * 100, label="spacing: 0")
plt.semilogx(requested_sizes, rel_waste_1 * 100, label="spacing: 1")
plt.semilogx(requested_sizes, rel_waste_2 * 100, label="spacing: 2")
plt.semilogx(requested_sizes, rel_waste_3 * 100, label="spacing: 3")
plt.semilogx(requested_sizes, rel_waste_4 * 100, label="spacing: 4")
plt.semilogx(requested_sizes, rel_waste_exp * 100, label="spacing: exp")
plt.semilogx(requested_sizes, rel_waste_var_pref * 100, label="var pref")
plt.title("Relative Waste")
plt.xlabel("Requested Memory")
plt.ylabel("Waste in %")
plt.legend();
plt.show(block=False)

print(f"padding 0  : mean waste: {np.mean(waste_0)   : 10.0f} byte, {np.mean(rel_waste_0[rel_waste_0>0])   * 100 : 4.1f} %")
print(f"padding 1  : mean waste: {np.mean(waste_1)   : 10.0f} byte, {np.mean(rel_waste_1[rel_waste_1>0])   * 100 : 4.1f} %")
print(f"padding 2  : mean waste: {np.mean(waste_2)   : 10.0f} byte, {np.mean(rel_waste_2[rel_waste_2>0])   * 100 : 4.1f} %")
print(f"padding 3  : mean waste: {np.mean(waste_3)   : 10.0f} byte, {np.mean(rel_waste_3[rel_waste_3>0])   * 100 : 4.1f} %")
print(f"padding 4  : mean waste: {np.mean(waste_4)   : 10.0f} byte, {np.mean(rel_waste_4[rel_waste_4>0])   * 100 : 4.1f} %")
print(f"padding exp: mean waste: {np.mean(waste_exp) : 10.0f} byte, {np.mean(rel_waste_exp[rel_waste_exp>0]) * 100 : 4.1f} %")
print(f"var prefix : mean waste: {np.mean(waste_var_pref) : 10.0f} byte, {np.mean(rel_waste_var_pref[rel_waste_var_pref>0]) * 100 : 4.1f} %")

#
# Just compare spacing 3 and exp
#

# Plot 1: Requested size vs allocated size
plt.figure()
plt.loglog(requested_sizes, allocated_sizes_spacing_3, label="spacing: 3")
plt.loglog(requested_sizes, allocated_sizes_spacing_exp, label="spacing: exp")
plt.loglog(requested_sizes, allocated_sizes_variable_prefix, label="var pref")
plt.title("Decoded Size vs Encoded Value")
plt.xlabel("Requested Memory")
plt.ylabel("Allocated Memory")
plt.legend();
plt.show(block=False)

plt.figure()
plt.loglog(requested_sizes, waste_3, label="spacing: 3")
plt.loglog(requested_sizes, waste_exp, label="spacing: exp")
plt.loglog(requested_sizes, waste_var_pref, label="var pref")
plt.title("Waste")
plt.xlabel("Requested Memory")
plt.ylabel("Waste in byte")
plt.legend();
plt.show(block=False)

# Plot 3: Waste in %
plt.figure()
plt.semilogx(requested_sizes, rel_waste_3 * 100, label="spacing: 3")
plt.semilogx(requested_sizes, rel_waste_exp * 100, label="spacing: exp")
plt.loglog(requested_sizes, rel_waste_var_pref * 100, label="var pref")
plt.title("Relative Waste")
plt.xlabel("Requested Memory")
plt.ylabel("Waste in %")
plt.legend();
plt.show()