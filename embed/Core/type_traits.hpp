
#pragma once

#include <cstdint>

namespace embed
{
// ---------------------------------------------------------------------------
//                              make_fast
// ---------------------------------------------------------------------------
    template<std::integral UInt> 
    struct make_fast{};

    template<std::unsigned_integral UInt> 
    requires (sizeof(UInt) == 1) 
    struct make_fast<UInt>{ 
        using type = uint_fast8_t; 
    };

    template<std::unsigned_integral UInt> 
    requires (sizeof(UInt) == 2) 
    struct make_fast<UInt>{ 
        using type = uint_fast16_t; 
    };

    template<std::unsigned_integral UInt> 
    requires (sizeof(UInt) == 4) 
    struct make_fast<UInt>{ 
        using type = uint_fast32_t; 
    };

    template<std::unsigned_integral UInt> 
    requires (sizeof(UInt) == 8) 
    struct make_fast<UInt>{ 
        using type = uint_fast64_t; 
    };




    template<std::signed_integral SInt> 
    requires (sizeof(SInt) == 1) 
    struct make_fast<SInt>{ 
        using type = int_fast8_t; 
    };

    template<std::signed_integral SInt> 
    requires (sizeof(SInt) == 2) 
    struct make_fast<SInt>{ 
        using type = int_fast16_t; 
    };

    template<std::signed_integral SInt> 
    requires (sizeof(SInt) == 4) 
    struct make_fast<SInt>{ 
        using type = int_fast32_t; 
    };

    template<std::signed_integral SInt> 
    requires (sizeof(SInt) == 8) 
    struct make_fast<SInt>{ 
        using type = int_fast64_t; 
    };
    
} // namespace embed
