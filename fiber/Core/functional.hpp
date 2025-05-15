#pragma once

#include <type_traits>


namespace fiber{

    template<class A, class B>
    constexpr bool equal(A a, B b){
        if constexpr (std::is_signed_v<A> && std::is_unsigned_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (a < 0) ? false : (static_cast<UInt>(a) == static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (a < 0) ? false : (static_cast<UInt>(a) == static_cast<UInt>(b));
            }
        }else if constexpr (std::is_unsigned_v<A> && std::is_signed_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (b < 0) ? false : (static_cast<UInt>(a) == static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (b < 0) ? false : (static_cast<UInt>(a) == static_cast<UInt>(b));
            }
        }else{
            return a == b;
        }
    }

    template<std::integral A, std::integral B>
    constexpr bool not_equal(A a, B b){
        if constexpr (std::is_signed_v<A> && std::is_unsigned_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (a < 0) ? true : (static_cast<UInt>(a) != static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (a < 0) ? true : (static_cast<UInt>(a) != static_cast<UInt>(b));
            }
        }else if constexpr (std::is_unsigned_v<A> && std::is_signed_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (b < 0) ? true : (static_cast<UInt>(a) != static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (b < 0) ? true : (static_cast<UInt>(a) != static_cast<UInt>(b));
            }
        }else{
            return a != b;
        }
    }
    
    template<std::integral A, std::integral B>
    constexpr bool less_equal(A a, B b){
        if constexpr (std::is_signed_v<A> && std::is_unsigned_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (a < 0) ? true : (static_cast<UInt>(a) <= static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (a < 0) ? true : (static_cast<UInt>(a) <= static_cast<UInt>(b));
            }
        }else if constexpr (std::is_unsigned_v<A> && std::is_signed_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (b < 0) ? false : (static_cast<UInt>(a) <= static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (b < 0) ? false : (static_cast<UInt>(a) <= static_cast<UInt>(b));
            }
        }else{
            return a <= b;
        }
    }

    template<std::integral A, std::integral B>
    constexpr bool less(A a, B b){
        if constexpr (std::is_signed_v<A> && std::is_unsigned_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (a < 0) ? true : (static_cast<UInt>(a) < static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (a < 0) ? true : (static_cast<UInt>(a) < static_cast<UInt>(b));
            }
        }else if constexpr (std::is_unsigned_v<A> && std::is_signed_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (b < 0) ? false : (static_cast<UInt>(a) < static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (b < 0) ? false : (static_cast<UInt>(a) < static_cast<UInt>(b));
            }
        }else{
            return a < b;
        }
    }

    template<std::integral A, std::integral B>
    constexpr bool greater_equal(A a, B b){
        if constexpr (std::is_signed_v<A> && std::is_unsigned_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (a < 0) ? false : (static_cast<UInt>(a) >= static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (a < 0) ? false : (static_cast<UInt>(a) >= static_cast<UInt>(b));
            }
        }else if constexpr (std::is_unsigned_v<A> && std::is_signed_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (b < 0) ? true : (static_cast<UInt>(a) >= static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (b < 0) ? true : (static_cast<UInt>(a) >= static_cast<UInt>(b));
            }
        }else{
            return a >= b;
        }
    }

    template<std::integral A, std::integral B>
    constexpr bool greater(A a, B b){
        if constexpr (std::is_signed_v<A> && std::is_unsigned_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (a < 0) ? false : (static_cast<UInt>(a) > static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (a < 0) ? false : (static_cast<UInt>(a) > static_cast<UInt>(b));
            }
        }else if constexpr (std::is_unsigned_v<A> && std::is_signed_v<B>){
            if(sizeof(A) > sizeof(B)){
                using UInt = std::make_unsigned<A>::type;
                return (b < 0) ? true : (static_cast<UInt>(a) > static_cast<UInt>(b));
            }else{
                using UInt = std::make_unsigned<B>::type;
                return (b < 0) ? true : (static_cast<UInt>(a) > static_cast<UInt>(b));
            }
        }else{
            return a > b;
        }
    }

} // namespace fiber
