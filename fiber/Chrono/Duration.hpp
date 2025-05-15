#pragma once

// std
#include <chrono>
#include <ratio>

// fiber
#include "Tick.hpp"
#include "rounding_duration_cast.hpp"
#include <fiber/OStream/OStream.hpp>

// just expose it so everyone can write 1ms.
// I know this is normally seen as smelly but for units I will make an exception here
using namespace std::chrono_literals;

namespace fiber
{

    /*
    Define the representation: `FIBER_CLOCK_UINT<x>`
    Define the frequency with: `FIBER_RTC_FREQ_HZ`
    */
    #if defined(FIBER_CLOCK_UINT8)
        using DurationRepresentation = uint8_t;
    #elif defined(FIBER_CLOCK_UINT16)
        using DurationRepresentation = uint16_t;
    #elif defined(FIBER_CLOCK_UINT32)
        using DurationRepresentation = uint32_t;
    #elif defined(FIBER_CLOCK_UINT64)
        using DurationRepresentation = uint64_t;
    #else
        #error "No clock representation has been set. S: Set one of the following definitions: `FIBER_CLOCK_UINT8`, `FIBER_CLOCK_UINT16`, `FIBER_CLOCK_UINT32` or `FIBER_CLOCK_UINT64`"
    #endif

    #if !defined(FIBER_RTC_FREQ_HZ)
        #error "No clock frequency has been set. S: Set `FIBER_RTC_FREQ_HZ` to an integer frequency"
    #endif

    /**
     * \brief Overflow aware duration for hardware timers/counters
     */
    class Duration : public std::chrono::duration<Tick<DurationRepresentation>, std::ratio<1, FIBER_RTC_FREQ_HZ>>{
        public:
        using base = std::chrono::duration<Tick<DurationRepresentation>, std::ratio<1, FIBER_RTC_FREQ_HZ>>;
        using rep = typename base::rep;
        using period = typename base::period;

        constexpr Duration() = default;
        constexpr Duration(const Duration&) = default;
        constexpr Duration& operator=(const Duration&) = default;

        constexpr Duration(rep value) : base(value){}

        constexpr Duration(DurationRepresentation value) : base(Tick<DurationRepresentation>(value)){}

        template<class Rep, CRatio Period>
        constexpr Duration(std::chrono::duration<Rep, Period> duration) 
            : base(fiber::rounding_duration_cast<base>(duration)){}


    };



    /**
     * \brief prints the time duration to the output stream
     */
    inline OStream& operator<<(OStream& stream, const Duration& duration){
        using Period = std::ratio<1, FIBER_RTC_FREQ_HZ>;

        // convert to standard duration so it can be easily cast with duration_cast
        const std::chrono::duration<DurationRepresentation, Period> d(static_cast<DurationRepresentation>(duration.count()));
        
        // depending on the duration -> downcast to next lower konventional period
        if constexpr (std::ratio_greater_equal<Period, typename std::chrono::weeks::period>::value){
            return stream << fiber::rounding_duration_cast<std::chrono::weeks>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::days::period>::value){
            return stream << fiber::rounding_duration_cast<std::chrono::days>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::hours::period>::value){
            return stream << fiber::rounding_duration_cast<std::chrono::hours>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::minutes::period>::value){
            return stream << fiber::rounding_duration_cast<std::chrono::minutes>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::seconds::period>::value){
            return stream << fiber::rounding_duration_cast<std::chrono::seconds>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::milliseconds::period>::value){
            return stream << fiber::rounding_duration_cast<std::chrono::milliseconds>(d);
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::microseconds::period>::value){
            return stream << fiber::rounding_duration_cast<std::chrono::microseconds>(d);
        }else /*if constexpr (std::ratio_greater_equal<Period, typename std::chrono::nanoseconds::period)) */{
            return stream << fiber::rounding_duration_cast<std::chrono::nanoseconds>(d);
        }
    }
} // namespace fiber
