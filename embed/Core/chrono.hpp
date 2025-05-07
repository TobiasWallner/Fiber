#pragma once

namespace embed
{

    /**
     * \brief duration cast that round to the nearest tick of the new `ToDuration`
     */
    template<class ToDuration, std::integral Rep, embed::CRatio Period>
    requires (std::is_integral_v<typename ToDuration::rep>)
    constexpr ToDuration rounding_duration_cast( const std::chrono::duration<Rep, Period>& d ){
        using conversion = typename std::ratio_divide<Period, typename ToDuration::period>::type;
        if constexpr (std::is_unsigned_v<Rep>){
            const auto mul = d.count() * conversion::num;
            const auto round = mul + conversion::den/2;
            const auto new_count = round / conversion::den;
            ToDuration result(static_cast<ToDuration::rep>(new_count));
            return result;
        }else{
            const auto mul = d.count() * conversion::num;
            const auto round = mul + ((d.count() > 0) ? conversion::den/2 : -conversion::den/2);
            const auto new_count = round / conversion::den;
            ToDuration result(static_cast<ToDuration::rep>(new_count));
            return result;
        }
    }
    
} // namespace embed

