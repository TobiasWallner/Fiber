#pragma once

// std
#include <chrono>

// embed
#include "Duration.hpp"
#include <embed/OStream/OStream.hpp>

namespace embed{

    /**
     * \brief Overflow aware point in time for hardware timers/counters
     */
    class TimePoint{
    private:
        Duration _duration;

    public:
        constexpr TimePoint() = default;
        constexpr TimePoint(const TimePoint&) = default;
        constexpr TimePoint& operator=(const TimePoint&) = default;
        explicit constexpr TimePoint(Duration duration) : _duration(duration){};
        
        template<class Rep, CRatio Period>
        explicit constexpr TimePoint(std::chrono::duration<Rep, Period> duration) 
            : _duration(embed::rounding_duration_cast<embed::Duration>(duration)){}

        constexpr TimePoint& operator+=(Duration d){this->_duration += d; return *this;}
        constexpr TimePoint& operator-=(Duration d){this->_duration -= d; return *this;}
        
        template<class Rep, CRatio Period>
        constexpr TimePoint& operator+=(std::chrono::duration<Rep, Period> d){this->_duration += embed::rounding_duration_cast<Duration>(d); return *this;}
        
        template<class Rep, CRatio Period>
        constexpr TimePoint& operator-=(Duration d){this->_duration -= embed::rounding_duration_cast<Duration>(d); return *this;}

        constexpr Duration time_since_epoch() const {return this->_duration;}

        friend constexpr TimePoint operator+ (TimePoint lhs, Duration rhs){
            const TimePoint result(lhs.time_since_epoch() + rhs);
            return result;
        }

        template<class Rep, CRatio Period>
        friend constexpr TimePoint operator+ (TimePoint lhs, std::chrono::duration<Rep, Period> rhs){
            return lhs + embed::rounding_duration_cast<Duration>(rhs);
        }

        friend constexpr TimePoint operator+ (Duration lhs, TimePoint rhs){
            const TimePoint result(lhs + rhs.time_since_epoch());
            return result;
        }

        template<class Rep, CRatio Period>
        friend constexpr TimePoint operator+ (std::chrono::duration<Rep, Period> lhs, TimePoint rhs){
            return embed::rounding_duration_cast<Duration>(lhs) + rhs;
        }

        friend constexpr TimePoint operator- (TimePoint lhs, Duration rhs){
            const TimePoint result(lhs.time_since_epoch() - rhs);
            return result;
        }

        template<class Rep, CRatio Period>
        friend constexpr TimePoint operator- (TimePoint lhs, std::chrono::duration<Rep, Period> rhs){
            return lhs - embed::rounding_duration_cast<Duration>(rhs);
        }

        friend constexpr TimePoint operator- (Duration lhs, TimePoint rhs){
            const TimePoint result(lhs - rhs.time_since_epoch());
            return result;
        }

        template<class Rep, CRatio Period>
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