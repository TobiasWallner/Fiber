
#pragma once

#include <chrono>
#include <type_traits>
#include <limits>
#include <concepts>
#include <ratio>

#include <embed/Core/definitions.hpp>
#include <embed/Core/concepts.hpp>
#include <embed/Exceptions/Exceptions.hpp>

// just expose it so everyone can write 1ms.
using namespace std::chrono_literals;

namespace embed
{

    /**
     * @brief An overflow aware tick class template to simulate the behaviour of hardware timers in microcontrollers for use in Duration and TimePoint types
     * @tparam UInt The underlieing integer type
     * @tparam MAX_TICK The largest/last value before the overflow.
     */
    template<std::unsigned_integral UInt, UInt MAX_TICK = std::numeric_limits<UInt>::max()>
    class ClockTick{
    public:

        using rep = UInt;

        UInt value = UInt(0);
        static constexpr UInt max_tick = MAX_TICK;
        static constexpr UInt modulo = static_cast<UInt>(max_tick + static_cast<UInt>(1));
        static constexpr bool modulo_is_power_of_two = (max_tick & modulo) == static_cast<UInt>(0);
        static constexpr bool max_tick_is_limit = std::numeric_limits<UInt>::max() == max_tick;

        constexpr ClockTick() = default;
        constexpr ClockTick(const ClockTick&) = default;
        constexpr ClockTick& operator=(const ClockTick&) = default;
        
        /**
         * @brief Safely constructs a ClockTick from the input value, so that the value wraps around after the `MAX_TICK`.
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
        template<std::unsigned_integral T>
        constexpr ClockTick(const T v){
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

        template<std::signed_integral T>
        constexpr ClockTick(const T v){
            if (v >= 0) {
                *this = ClockTick(static_cast<UInt>(v));
            }else{
                *this = -ClockTick(static_cast<UInt>(-v)); // use the equation: v mod n = (-(-v mod n)) mod n, for: v < 0, n > 0
            }
        }

        template<std::integral T>
        constexpr ClockTick& operator=(const T v){return *this = ClockTick(v);}

        /// @brief Reinterprets the integer value into the clock tick. this assumes that the passed value is smaller or equal than the overflow value
        /// @tparam Int Generic integer type
        /// @param v an integer
        /// @return a ClockTick
        /// @throws If full assertions are enabled, throws if the passed value is larger than the modulo
        template<std::unsigned_integral Int>
        static constexpr ClockTick reinterpret(const Int v){
            EMBED_ASSERT_FULL(v <= modulo);
            ClockTick result;
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
        friend constexpr ClockTick operator+(const ClockTick lhs, const ClockTick rhs){
            if constexpr (max_tick_is_limit){
                // if the overflow happens at the maximal representable value: just add them and let them wrap around by themselfes
                return ClockTick::reinterpret(static_cast<UInt>(lhs.value + rhs.value));
            }else if constexpr (modulo_is_power_of_two){
                // use a mask to do a quick modulo
                return ClockTick::reinterpret(static_cast<UInt>((lhs.value + rhs.value) & max_tick));
            }else{
                // if all else failes, fall back 
                const auto result = lhs.value + rhs.value;
                return  ClockTick::reinterpret(static_cast<UInt>((result < modulo) ? result : result - modulo));
            }
        }

        /**
         * @brief subtraction that wrapps around `MAX_TICKS`
         * 
         * Supports compile time optimisations
         *   - if the `MAX_TICKS` is the largest representable number (aka. natural overflow)
         *   - else falls back to subtraction with conditional modulo addition
         */
        friend constexpr ClockTick operator-(ClockTick lhs, ClockTick rhs){
            if constexpr (max_tick_is_limit){
                // if the overflow happens at the maximal representable value: just add them and let them wrap around by themselfes
                lhs.value -= rhs.value;
                return lhs;
            }else {
                // fallback
                const auto result = lhs.value - rhs.value;
                return ClockTick::reinterpret(static_cast<UInt>((lhs.value < rhs.value) ? result + modulo : result));
            }
        }

        /**
         * @brief unary + operator is just the identity function
         */
        friend constexpr ClockTick operator+(const ClockTick v){return v;}

        /**
         * @brief negation that wrapps around `MAX_TICKS`
         * 
         * Supports compile time optimisations
         *   - if the `MAX_TICKS` is the largest representable number (aka. natural overflow)
         *   - else falls back to subtraction from modulo
         */
        friend constexpr ClockTick operator-(const ClockTick v){
            if constexpr (max_tick_is_limit && (/* stupid integer promotion rules */sizeof(UInt) >= sizeof(int))){
                // if the overflow happens at the maximal representable value: just add them and let them wrap around by themselfes
                return ClockTick::reinterpret(-v.value);
            } else {
                // wrap around
                return ClockTick::reinterpret(static_cast<UInt>(modulo - v.value));
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
        friend constexpr ClockTick operator*(const ClockTick lhs, const ClockTick rhs){
            if constexpr (max_tick_is_limit){
                // if the overflow happens at the maximal representable value: just add them and let them wrap around by themselfes
                return ClockTick::reinterpret(static_cast<UInt>(lhs.value * rhs.value));
            }else if constexpr (modulo_is_power_of_two){
                return ClockTick::reinterpret(static_cast<UInt>((lhs.value * rhs.value) & max_tick));
            }else {
                // do the modulo and pray that the compiler can optimize it away
                return ClockTick::reinterpret(static_cast<UInt>((lhs.value * rhs.value) % modulo));
            }
        }

        /**
         * @brief division that wrapps around `MAX_TICKS`
         */
        friend constexpr ClockTick operator/(const ClockTick lhs, const ClockTick rhs){
            // the number can only get smaller
            return ClockTick::reinterpret(static_cast<UInt>(lhs.value / rhs.value));
        }

        /**
         * @brief remainder/modulo opteration thta wraps around `MAX_TICKS`
         */
        friend constexpr ClockTick operator%(const ClockTick lhs, const ClockTick rhs){
            // the number can only get smaller
            return ClockTick::reinterpret(static_cast<UInt>(lhs.value % rhs.value));
        }

        inline ClockTick& operator+=(const ClockTick other){*this = *this + other;}
        inline ClockTick& operator-=(const ClockTick other){*this = *this - other;}
        inline ClockTick& operator*=(const ClockTick other){*this = *this * other;}
        inline ClockTick& operator/=(const ClockTick other){*this = *this / other;}
        inline ClockTick& operator%=(const ClockTick other){*this = *this % other;}

        friend constexpr bool operator==(const ClockTick lhs, const ClockTick rhs){return lhs.value == rhs.value;}
        friend constexpr bool operator!=(const ClockTick lhs, const ClockTick rhs){return lhs.value != rhs.value;}

        friend constexpr bool operator<=(const ClockTick lhs, const ClockTick rhs){
            const auto distance = (lhs.value <= rhs.value) ? rhs.value - lhs.value : rhs.value + max_tick + 1 -lhs.value;
            const bool result = distance < (max_tick/2);
            return result;
        }

        friend constexpr bool operator>=(const ClockTick lhs, const ClockTick rhs){return rhs <= lhs;}
        friend constexpr bool operator<(const ClockTick lhs, const ClockTick rhs){return !(lhs >= rhs);}
        friend constexpr bool operator>(const ClockTick lhs, const ClockTick rhs){return !(lhs <= rhs);}
        
        template<std::integral Int>
        explicit constexpr operator Int () const {return static_cast<Int>(this->value);}

        friend OStream& operator<<(OStream& stream, const ClockTick tick){return stream << tick.value;}
    };

    

    /// @brief Overflow aware time duration type
    /// @tparam UInt The underlieing integral type for the counter
    /// @tparam MaxTick Maximum distance before timer overflow
    /// @tparam Period std::ratio to 1s
    template<std::unsigned_integral UInt, CStdRatio Period = std::ratio<1>, UInt MaxTick = std::numeric_limits<UInt>::max()>
    using Duration = std::chrono::duration<ClockTick<UInt, MaxTick>, Period>;

    template<std::unsigned_integral UInt, CStdRatio Period = std::ratio<1>, UInt MaxTick = std::numeric_limits<UInt>::max()>
    OStream& operator<<(OStream& stream, const Duration<UInt, Period, MaxTick>& duration){
        // convert to simple standard duration so it can be easily cast with duration_cast
        const std::chrono::duration<long long, Period> d(static_cast<long long>(duration.count()));
        
        // depending on the duration -> downcast to next lower konventional period
        if constexpr (std::ratio_greater_equal<Period, typename std::chrono::years::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::years>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::months::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::month>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::weeks::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::weeks>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::days::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::days>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::hours::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::hours>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::minutes::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::minutes>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::seconds::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::seconds>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::milliseconds::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::milliseconds>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::microseconds::period>::value){
            return stream << std::chrono::duration_cast<std::chrono::microseconds>(d);
        }else /*if constexpr (std::ratio_greater_equal<Period, typename std::chrono::nanoseconds::period)) */{
            return stream << std::chrono::duration_cast<std::chrono::nanoseconds>(d);
        }
    }

    /// @brief Overflow aware time point
    /// @tparam Clock The clock this time point referes to
    /// @tparam UInt The underlieing integral type for the counter
    /// @tparam MaxTick Maximum distance before timer overflow
    /// @tparam Period Period std::ratio to 1s
    template<class Clock, std::unsigned_integral UInt=Clock::value_type, CStdRatio Period = Clock::period, UInt MaxTick=Clock::max_tick>
    using TimePoint = std::chrono::time_point<Clock, Duration<UInt, Period, MaxTick>>;

    template<class Clock, std::unsigned_integral UInt=Clock::value_type, CStdRatio Period = Clock::period, UInt MaxTick=Clock::max_tick>
    OStream& operator<<(OStream& stream, const TimePoint<Clock, UInt, Period, MaxTick>& tp){
        return stream << tp.time_since_epoch();
    }

    /**
     * @brief Concept for a clock with overflow awareness
     */
    template<class C>
    concept CClock = requires {
        typename C::value_type;
        typename C::rep;
        typename C::period;
        typename C::duration;
        typename C::time_point;
        { C::max_tick } -> std::convertible_to<typename C::rep>;
        { C::now() } -> std::same_as<typename C::time_point>;
    }   && std::unsigned_integral<typename C::value_type> 
        && std::same_as<typename C::rep, ClockTick<typename C::value_type, C::max_tick>> 
        && CStdRatio<typename C::period> 
        && std::same_as<typename C::duration, Duration<typename C::value_type, typename C::period, C::max_tick>> 
        && std::same_as<typename C::time_point, TimePoint<C, typename C::value_type, typename C::period, C::max_tick>>;

    /** 
     * @brief A template for an overflow aware clock type for the use with `TimePoint`
     * 
     * Example for a hardware timer that uses a 32-bit counter at 1MHz
     * ```
     * inline uint32_t get_timer1_count(){
     *   return timer1_handle->counter_register;
     * }
     * 
     * using MyClock = embed::Clock<uint32_t, 0xFFFFFFFF, std::milli, get_timer1_count>;
     * 
     * TimePoint<MyClock> tp;
     * ```
     * 
     * 
     * @tparam UInt The integer type of the counter/timer
     * @tparam MaxTick The maximum tick value of the timer before the overflow
     * @tparam Period The ratio for 1s
     * @tparam GetTimerCount A function to the hardware timer that returns the timer count as an (preferably) unsigned integral
     */
    template<std::unsigned_integral UInt, CStdRatio Period, UInt (*GetTimerCount)(void), UInt MaxTick = std::numeric_limits<UInt>::max()>
    struct Clock{
        using value_type = UInt;
        using rep = ClockTick<UInt, MaxTick>;
        using period = Period;
        using duration = Duration<UInt, Period, MaxTick>;
        using time_point = TimePoint<Clock, UInt, Period, MaxTick>;
        
        static constexpr inline UInt max_tick = MaxTick;
        
        static time_point now() noexcept {
            const UInt timer_count = GetTimerCount();
            const duration dur(timer_count);
            const time_point result(dur);
            return result;
        }
    };

    
    
} // namespace embed
