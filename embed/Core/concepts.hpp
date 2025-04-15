#pragma once

#include <type_traits>

namespace embed
{
    
    template<class T>
    concept CStringView = requires(T t) {
        t.data();
        t.size();
    };

// --------------------------------------------------------------------------------
//                               CStdRatio
// --------------------------------------------------------------------------------

    template<typename T>
    struct is_std_ratio : std::false_type {};

    // Specialization for std::ratio<num, den>
    template<intmax_t Num, intmax_t Den>
    struct is_std_ratio<std::ratio<Num, Den>> : std::true_type {};

    template<class T>
    constexpr inline bool is_std_ratio_v = is_std_ratio<T>::value;

    template<typename T>
    concept CStdRatio = is_std_ratio_v<T>;

// --------------------------------------------------------------------------------
//                               CStdOptional
// --------------------------------------------------------------------------------


    template<typename T>
    struct is_std_optional : std::false_type {};

    // Specialization for std::ratio<num, den>
    template<class T>
    struct is_std_optional<std::optional<T>> : std::true_type {};

    template<class T>
    constexpr inline bool is_std_optional_v = is_std_optional<T>::value;

    template<typename T>
    concept CStdOptional = is_std_optional<T>::value;

} // namespace embed


