#pragma once

#include <fiber/Interfaces/cPin.hpp>

#include "GPIO.hpp"

namespace fiber
{
    namespace stm32f4{

        class Pin{
            private:

            

            GPIO* _GPIOx;
            uint16_t _GPIO_Pin;

            public:

            constexpr Pin(GPIO* GPIOx, uint16_t GPIO_Pin)
                : _GPIOx(GPIOx)
                , _GPIO_Pin(GPIO_Pin)
                {}

            constexpr Pin(const Pin& other) = default;
            
            constexpr Pin& operator=(const Pin& other) = default;

            inline void high(){

            }

            inline void low(){

            }

            inline void set(bool level){
                
            }

            inline void input(){

            }

            inline void output(){

            }

            inline bool read(){
                
            }

        };

        static_assert(requires(fiber::cPin<Pin>));

        class InPin{
            private:



            public:
        };

        class OutPin{

        };

    } // namespace stm32f4
    

} // namespace fiber
