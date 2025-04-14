#pragma once

#include <type_traits>

namespace embed
{
    
    template<class T>
    concept CStringView = requires(T t) {
        t.data();
        t.size();
    };

    template<typename T>
    struct is_std_ratio : std::false_type {};

    // Specialization for std::ratio<num, den>
    template<intmax_t Num, intmax_t Den>
    struct is_std_ratio<std::ratio<Num, Den>> : std::true_type {};

    template<class T>
    constexpr inline bool is_std_ratio_v = is_std_ratio<T>::value;

    template<typename T>
    concept CStdRatio = is_std_ratio<T>::value;

} // namespace embed


