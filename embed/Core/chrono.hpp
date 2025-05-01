#pragma once

namespace embed
{
    
    /**
     * \brief duration cast that round to the nearest tick of the new `ToDuration`
     */
    template<embed::CStdDuration ToDuration, std::integral Rep, embed::CStdRatio Period>
    requires (std::is_integral_v<typename ToDuration::rep>)
    constexpr ToDuration rounding_duration_cast( const std::chrono::duration<Rep, Period>& d ){
        using conversion = typename std::ratio_divide<Period, typename ToDuration::period>::type;
        if constexpr (std::is_unsigned_v<Rep>){
            const auto new_count = (d.count() + conversion::den/2) * conversion::num / conversion::den;
            return ToDuration(new_count);
        }else{
            const auto new_count = (d.count() + ((d.count() > 0) ? conversion::den/2 : -conversion::den/2)) * conversion::num / conversion::den;
            return ToDuration(new_count);
        }
    }
    
} // namespace embed

