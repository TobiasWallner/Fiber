
#pragma once

// std
#include <limits>
#include <concepts>
#include <type_traits>

// fiber
#include <fiber/OStream/OStream.hpp>

namespace fiber{

    /**
     * \brief An overflow aware integer type for hardware timer/counter that are used as clocks.
     * 
     * - Allows `<`, `<=`, `>=` and `>` comparisons at and around the overflow point.
     * - Allows `+`, `-`, `*`, `/` and `%` operations at and around the overflow point.
     * 
     * > Note: The maximum comparison range is half the `MAX_TICK` range
     * 
     * \tparam UInt An unsigned integer type
     * \tparam MAX_TICK The maximum tick just before the overflow
     */
    template<std::unsigned_integral UInt, UInt MAX_TICK = std::numeric_limits<UInt>::max()>
    class Tick;

    template<typename T>
    inline constexpr bool is_clocktick = false;

    template<typename U, U Max>
    inline constexpr bool is_clocktick<Tick<U, Max>> = true;

    /**
     * @brief An overflow aware tick class template to simulate the behaviour of hardware timers in microcontrollers for use in Duration and TimePoint types
     * @tparam UInt The underlieing integer type
     * @tparam MAX_TICK The largest/last value before the overflow.
     */
    template<std::unsigned_integral UInt, UInt MAX_TICK>
    class Tick{
    public:

        using rep = UInt;

        UInt value = UInt(0);
        static constexpr UInt max_tick = MAX_TICK;
        static constexpr UInt modulo = static_cast<UInt>(max_tick + static_cast<UInt>(1));
        static constexpr bool modulo_is_power_of_two = (max_tick & modulo) == static_cast<UInt>(0);
        static constexpr bool max_tick_is_limit = std::numeric_limits<UInt>::max() == max_tick;

        constexpr Tick() = default;
        constexpr Tick(const Tick&) = default;
        constexpr Tick& operator=(const Tick&) = default;
        
        /**
         * @brief Safely constructs a Tick from the input value, so that the value wraps around after the `MAX_TICK`.
         * 
         * 
         * Compile time optimisations:
         * ---------------------------
         * - If the passed type `T` is fully representable within `MAX_TICK`: compile to a simple assignment.
         * - Fallback 1: `MAX_TICK + 1` is a power of two: compiles to an bitwise-and in addition to the assignment.
         * - Fallback 2: `T` is fully representable within `2 * MAX_TICK`: compiles to a conditional subtraction.
         * - Fallback 3: applies the modulo operator
         * 
         * @tparam T Generic unsigned integer type
         * @param v unsigned integer value
         */
        template<class T>
        requires(!is_clocktick<T> && std::is_unsigned_v<T>)
        constexpr Tick(const T v){
            // try as much as possible to avoid '%'
            if constexpr (std::numeric_limits<T>::max() <= max_tick){
                this->value = static_cast<UInt>(v);
            } else if constexpr (modulo_is_power_of_two){
                this->value = static_cast<UInt>(v & static_cast<T>(max_tick));
            } else if constexpr (std::numeric_limits<T>::max()/2 <= max_tick){
                this->value = static_cast<UInt>((v < modulo) ? v : v-modulo);
            }else if (v <= max_tick) {
                this->value = static_cast<UInt>(v);
            }else{
                this->value = static_cast<UInt>(v % modulo);
            }
        }

        template<class T>
        requires(!is_clocktick<T> && std::is_signed_v<T>)
        constexpr Tick(const T v){
            if (v >= 0) {
                *this = Tick(static_cast<UInt>(v));
            }else{
                *this = -Tick(static_cast<UInt>(-v)); // use the equation: v mod n = (-(-v mod n)) mod n, for: v < 0, n > 0
            }
        }

        template<std::integral T>
        constexpr Tick& operator=(const T v){return *this = Tick(v);}

        /// @brief Reinterprets the integer value into the clock tick. this assumes that the passed value is smaller or equal than the overflow value
        /// @param v an integer
        /// @return a Tick
        /// @throws If full assertions are enabled, throws if the passed value is larger than the modulo
        static constexpr Tick<UInt, MAX_TICK> reinterpret(UInt v){
            FIBER_ASSERT_FULL(v <= modulo);
            Tick<UInt, MAX_TICK> result;
            result.value = static_cast<UInt>(v);
            return result;
        }

        /**
         * @brief addition that wrapps around `MAX_TICKS`
         * 
         * Supports compile time optimisations 
         *   - if the `MAX_TICKS` is the largest representable number (aka. natural overflow)
         *   - if `MAX_TICKS + 1` is a power of two
         *   - else falls back to addition with conditional modulo subtraction
         */
        friend constexpr Tick operator+(const Tick lhs, const Tick rhs){
            if constexpr (max_tick_is_limit){
                // if the overflow happens at the maximal representable value: just add them and let them wrap around by themselfes
                return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(lhs.value + rhs.value));
            }else if constexpr (modulo_is_power_of_two){
                // use a mask to do a quick modulo
                return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((lhs.value + rhs.value) & max_tick));
            }else{
                // if all else failes, fall back 
                const auto result = lhs.value + rhs.value;
                return  Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((result < modulo) ? result : result - modulo));
            }
        }

        /**
         * @brief subtraction that wrapps around `MAX_TICKS`
         * 
         * Supports compile time optimisations
         *   - if the `MAX_TICKS` is the largest representable number (aka. natural overflow)
         *   - else falls back to subtraction with conditional modulo addition
         */
        friend constexpr Tick operator-(Tick lhs, Tick rhs){
            if constexpr (max_tick_is_limit){
                // if the overflow happens at the maximal representable value: just add them and let them wrap around by themselfes
                lhs.value -= rhs.value;
                return lhs;
            }else {
                // fallback
                const auto result = lhs.value - rhs.value;
                return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((lhs.value < rhs.value) ? result + modulo : result));
            }
        }

        /**
         * @brief unary + operator is just the identity function
         */
        friend constexpr Tick operator+(const Tick v){return v;}

        /**
         * @brief negation that wrapps around `MAX_TICKS`
         * 
         * Supports compile time optimisations
         *   - if the `MAX_TICKS` is the largest representable number (aka. natural overflow)
         *   - else falls back to subtraction from modulo
         */
        friend constexpr Tick operator-(const Tick v){
            if constexpr (max_tick_is_limit && (/* stupid integer promotion rules */sizeof(UInt) >= sizeof(int))){
                // if the overflow happens at the maximal representable value: just add them and let them wrap around by themselfes
                return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(-v.value));
            } else {
                // wrap around
                return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(modulo - v.value));
            }
        }

        /**
         * @brief multiplication that wrapps around `MAX_TICKS`
         * 
         * Supports compile time optimisations 
         *   - if the `MAX_TICKS` is the largest representable number (aka. natural overflow)
         *   - if `MAX_TICKS + 1` is a power of two: multiplication and bitwise addition.
         *   - else falls back to addition with actual modulo operation - Tip: avoid or pray the compiler can optimise it away
         */
        friend constexpr Tick operator*(const Tick lhs, const Tick rhs){
            if constexpr (max_tick_is_limit){
                // if the overflow happens at the maximal representable value: just add them and let them wrap around by themselfes
                return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(lhs.value * rhs.value));
            }else if constexpr (modulo_is_power_of_two){
                return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((lhs.value * rhs.value) & max_tick));
            }else {
                // do the modulo and pray that the compiler can optimize it away
                return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((lhs.value * rhs.value) % modulo));
            }
        }

        /**
         * @brief division that wrapps around `MAX_TICKS`
         */
        friend constexpr Tick operator/(const Tick lhs, const Tick rhs){
            // the number can only get smaller
            return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(lhs.value / rhs.value));
        }

        /**
         * @brief remainder/modulo opteration thta wraps around `MAX_TICKS`
         */
        friend constexpr Tick operator%(const Tick lhs, const Tick rhs){
            // the number can only get smaller
            return Tick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(lhs.value % rhs.value));
        }

        inline void operator+=(const Tick other){*this = *this + other;}
        inline void operator-=(const Tick other){*this = *this - other;}
        inline void operator*=(const Tick other){*this = *this * other;}
        inline void operator/=(const Tick other){*this = *this / other;}
        inline void operator%=(const Tick other){*this = *this % other;}

        friend constexpr bool operator==(const Tick lhs, const Tick rhs){return lhs.value == rhs.value;}
        friend constexpr bool operator!=(const Tick lhs, const Tick rhs){return lhs.value != rhs.value;}

        friend constexpr bool operator<=(const Tick lhs, const Tick rhs){
            const auto distance = (lhs.value <= rhs.value) ? rhs.value - lhs.value : rhs.value + max_tick + 1 -lhs.value;
            const bool result = distance < (max_tick/2);
            return result;
        }

        friend constexpr bool operator>=(const Tick lhs, const Tick rhs){return rhs <= lhs;}
        friend constexpr bool operator<(const Tick lhs, const Tick rhs){return !(lhs >= rhs);}
        friend constexpr bool operator>(const Tick lhs, const Tick rhs){return !(lhs <= rhs);}
        
        template<std::integral Int>
        explicit constexpr operator Int () const {return static_cast<Int>(this->value);}

        friend OStream& operator<<(OStream& stream, const Tick tick){return stream << tick.value;}
    };
}// namespace fiber

namespace std{

    /// @brief Overload `std::is_integral` with `std::true_type` for `fiber::Tick`
    /// @tparam UInt an integer/integral type
    /// @tparam MaxTick the maximum number of representable ticks before overflow
    template<std::integral Int, Int MaxTick>
    struct is_integral<fiber::Tick<Int, MaxTick>> : std::true_type{};

    /// @brief Overload `std::is_unsigned` with `std::true_type` for `fiber::Tick`
    /// @tparam UInt an unsigned integer/integral
    /// @tparam MaxTick the maximum number of representable ticks before overflow
    template<std::unsigned_integral UInt, UInt MaxTick>
    struct is_unsigned<fiber::Tick<UInt, MaxTick>> : std::true_type{};
}