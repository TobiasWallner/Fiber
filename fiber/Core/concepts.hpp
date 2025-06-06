#pragma once

#include <type_traits>
#include <optional>

namespace fiber
{
    
    template<class T>
    concept CStringView = requires(T t) {
        t.data();
        t.size();
    };

// --------------------------------------------------------------------------------
//                               CRatio
// --------------------------------------------------------------------------------

    template<typename T>
    concept CRatio = requires {
        { T::num } -> std::convertible_to<intmax_t>;
        { T::den } -> std::convertible_to<intmax_t>;
    };

// --------------------------------------------------------------------------------
//                               CStdDuration
// --------------------------------------------------------------------------------

    template<typename T>
    concept CStdDuration = requires {
        typename T::rep;
        typename T::period;
        requires std::same_as<T, std::chrono::duration<typename T::rep, typename T::period>>;
    };

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

// --------------------------------------------------------------------------------
//                               AwaitableLike
// --------------------------------------------------------------------------------

    template<typename T>
    concept AwaitableLike = requires(T t) {
        { t.await_ready() } -> std::same_as<bool>;   // await_ready must return bool
        { t.await_resume() };                        // await_resume can return anything
    };

} // namespace fiber


