
#pragma once

#include <chrono>
#include <type_traits>
#include <limits>
#include <concepts>
#include <ratio>

#include <embed/Core/definitions.hpp>
#include <embed/Core/concepts.hpp>
#include <embed/Core/chrono.hpp>
#include <embed/Exceptions/Exceptions.hpp>
#include <embed/OStream/OStream.hpp>

// just expose it so everyone can write 1ms.
using namespace std::chrono_literals;

namespace embed{

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
        /// @param v an integer
        /// @return a ClockTick
        /// @throws If full assertions are enabled, throws if the passed value is larger than the modulo
        static constexpr ClockTick<UInt, MAX_TICK> reinterpret(UInt v){
            EMBED_ASSERT_FULL(v <= modulo);
            ClockTick<UInt, MAX_TICK> result;
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
                return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(lhs.value + rhs.value));
            }else if constexpr (modulo_is_power_of_two){
                // use a mask to do a quick modulo
                return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((lhs.value + rhs.value) & max_tick));
            }else{
                // if all else failes, fall back 
                const auto result = lhs.value + rhs.value;
                return  ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((result < modulo) ? result : result - modulo));
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
                return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((lhs.value < rhs.value) ? result + modulo : result));
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
                return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(-v.value));
            } else {
                // wrap around
                return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(modulo - v.value));
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
                return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(lhs.value * rhs.value));
            }else if constexpr (modulo_is_power_of_two){
                return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((lhs.value * rhs.value) & max_tick));
            }else {
                // do the modulo and pray that the compiler can optimize it away
                return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>((lhs.value * rhs.value) % modulo));
            }
        }

        /**
         * @brief division that wrapps around `MAX_TICKS`
         */
        friend constexpr ClockTick operator/(const ClockTick lhs, const ClockTick rhs){
            // the number can only get smaller
            return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(lhs.value / rhs.value));
        }

        /**
         * @brief remainder/modulo opteration thta wraps around `MAX_TICKS`
         */
        friend constexpr ClockTick operator%(const ClockTick lhs, const ClockTick rhs){
            // the number can only get smaller
            return ClockTick<UInt, MAX_TICK>::reinterpret(static_cast<UInt>(lhs.value % rhs.value));
        }

        inline void operator+=(const ClockTick other){*this = *this + other;}
        inline void operator-=(const ClockTick other){*this = *this - other;}
        inline void operator*=(const ClockTick other){*this = *this * other;}
        inline void operator/=(const ClockTick other){*this = *this / other;}
        inline void operator%=(const ClockTick other){*this = *this % other;}

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
}// namespace embed

namespace std{

    /// @brief Overload `std::is_integral` with `std::true_type` for `embed::ClockTick`
    /// @tparam UInt an integer/integral type
    /// @tparam MaxTick the maximum number of representable ticks before overflow
    template<std::integral Int, Int MaxTick>
    struct is_integral<embed::ClockTick<Int, MaxTick>> : std::true_type{};

    /// @brief Overload `std::is_unsigned` with `std::true_type` for `embed::ClockTick`
    /// @tparam UInt an unsigned integer/integral
    /// @tparam MaxTick the maximum number of representable ticks before overflow
    template<std::unsigned_integral UInt, UInt MaxTick>
    struct is_unsigned<embed::ClockTick<UInt, MaxTick>> : std::true_type{};

}

namespace embed{


    #if defined(EMBED_CLOCK_UINT8)
        using DurationRepresentation = uint8_t;
    #elif defined(EMBED_CLOCK_UINT16)
        using DurationRepresentation = uint16_t;
    #elif defined(EMBED_CLOCK_UINT32)
        using DurationRepresentation = uint32_t;
    #elif defined(EMBED_CLOCK_UINT64)
        using DurationRepresentation = uint64_t;
    #else
        #error "No clock representation has been set. S: Set one of the following definitions: `EMBED_CLOCK_UINT8`, `EMBED_CLOCK_UINT16`, `EMBED_CLOCK_UINT32` or `EMBED_CLOCK_UINT64`"
    #endif

    #if !defined(EMBED_RTC_FREQ_HZ)
        #error "No clock frequency has been set. S: Set `EMBED_RTC_FREQ_HZ` to an integer frequency"
    #endif

    /**
     * \brief Representation of a time duration
     */
    class Duration : public std::chrono::duration<ClockTick<DurationRepresentation>, std::ratio<1, EMBED_RTC_FREQ_HZ>>{
        public:
        using base = std::chrono::duration<ClockTick<DurationRepresentation>, std::ratio<1, EMBED_RTC_FREQ_HZ>>;
        using rep = typename base::rep;
        using period = typename base::period;

        constexpr Duration() = default;
        constexpr Duration(const Duration&) = default;

        constexpr Duration(rep value) : base(value){}

        constexpr Duration(DurationRepresentation value) : base(value){}

        template<std::integral Rep, CRatio Period>
        constexpr Duration(std::chrono::duration<Rep, Period> duration) 
            : base(embed::rounding_duration_cast<base>(duration)){}


    };



    /**
     * \brief prints the time duration to the output stream
     */
    inline OStream& operator<<(OStream& stream, const Duration& duration){
        using Period = std::ratio<1, EMBED_RTC_FREQ_HZ>;

        // convert to standard duration so it can be easily cast with duration_cast
        const std::chrono::duration<DurationRepresentation, Period> d(static_cast<DurationRepresentation>(duration.count()));
        
        // depending on the duration -> downcast to next lower konventional period
        if constexpr (std::ratio_greater_equal<Period, typename std::chrono::weeks::period>::value){
            return stream << embed::rounding_duration_cast<std::chrono::weeks>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::days::period>::value){
            return stream << embed::rounding_duration_cast<std::chrono::days>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::hours::period>::value){
            return stream << embed::rounding_duration_cast<std::chrono::hours>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::minutes::period>::value){
            return stream << embed::rounding_duration_cast<std::chrono::minutes>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::seconds::period>::value){
            return stream << embed::rounding_duration_cast<std::chrono::seconds>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::milliseconds::period>::value){
            return stream << embed::rounding_duration_cast<std::chrono::milliseconds>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::microseconds::period>::value){
            return stream << embed::rounding_duration_cast<std::chrono::microseconds>(d);
        }else /*if constexpr (std::ratio_greater_equal<Period, typename std::chrono::nanoseconds::period)) */{
            return stream << embed::rounding_duration_cast<std::chrono::nanoseconds>(d);
        }
    }

    /**
     * \brief Representation of a time point
     */
    class TimePoint{
    private:
        Duration _duration;

    public:
        TimePoint() = default;
        TimePoint(const TimePoint&) = default;
        explicit constexpr TimePoint(Duration duration) : _duration(duration){};
        
        template<std::integral Rep, CRatio Period>
        explicit constexpr TimePoint(std::chrono::duration<Rep, Period> duration) 
            : _duration(embed::rounding_duration_cast<embed::Duration>(duration)){}

        constexpr TimePoint& operator+=(Duration d){this->_duration += d; return *this;}
        constexpr TimePoint& operator-=(Duration d){this->_duration -= d; return *this;}
        
        template<std::integral Rep, CRatio Period>
        constexpr TimePoint& operator+=(std::chrono::duration<Rep, Period> d){this->_duration += embed::rounding_duration_cast<Duration>(d); return *this;}
        
        template<std::integral Rep, CRatio Period>
        constexpr TimePoint& operator-=(Duration d){this->_duration -= embed::rounding_duration_cast<Duration>(d); return *this;}

        constexpr Duration time_since_epoch() const {return this->_duration;}

        friend constexpr TimePoint operator+ (TimePoint lhs, Duration rhs){
            const TimePoint result(lhs.time_since_epoch() + rhs);
            return result;
        }

        template<std::integral Rep, CRatio Period>
        friend constexpr TimePoint operator+ (TimePoint lhs, std::chrono::duration<Rep, Period> rhs){
            return lhs + embed::rounding_duration_cast<Duration>(rhs);
        }

        friend constexpr TimePoint operator+ (Duration lhs, TimePoint rhs){
            const TimePoint result(lhs + rhs.time_since_epoch());
            return result;
        }

        template<std::integral Rep, CRatio Period>
        friend constexpr TimePoint operator+ (std::chrono::duration<Rep, Period> lhs, TimePoint rhs){
            return embed::rounding_duration_cast<Duration>(lhs) + rhs;
        }

        friend constexpr TimePoint operator- (TimePoint lhs, Duration rhs){
            const TimePoint result(lhs.time_since_epoch() - rhs);
            return result;
        }

        template<std::integral Rep, CRatio Period>
        friend constexpr TimePoint operator- (TimePoint lhs, std::chrono::duration<Rep, Period> rhs){
            return lhs - embed::rounding_duration_cast<Duration>(rhs);
        }

        friend constexpr TimePoint operator- (Duration lhs, TimePoint rhs){
            const TimePoint result(lhs - rhs.time_since_epoch());
            return result;
        }

        template<std::integral Rep, CRatio Period>
        friend constexpr TimePoint operator- (std::chrono::duration<Rep, Period> lhs, TimePoint rhs){
            return embed::rounding_duration_cast<Duration>(lhs) - rhs;
        }

        friend constexpr Duration operator- (TimePoint lhs, TimePoint rhs){
            const Duration result = lhs.time_since_epoch() - rhs.time_since_epoch();
            return result;
        }

        friend constexpr bool operator==(TimePoint lhs, TimePoint rhs){return lhs._duration == rhs._duration;}
        friend constexpr bool operator!=(TimePoint lhs, TimePoint rhs){return lhs._duration != rhs._duration;}

        friend constexpr bool operator<=(TimePoint lhs, TimePoint rhs){return lhs._duration <= rhs._duration;}
        friend constexpr bool operator>=(TimePoint lhs, TimePoint rhs){return lhs._duration >= rhs._duration;}
        friend constexpr bool operator<(TimePoint lhs, TimePoint rhs){return lhs._duration < rhs._duration;}
        friend constexpr bool operator>(TimePoint lhs, TimePoint rhs){return lhs._duration > rhs._duration;}
    };

    /**
     * \brief prints the time point to the output stream
     */
    inline OStream& operator<<(OStream& stream, const TimePoint& time){
        return stream << time.time_since_epoch();
    }

} // namespace embed
