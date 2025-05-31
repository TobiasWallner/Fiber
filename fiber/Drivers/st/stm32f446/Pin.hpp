#pragma once

/**
 * \file Pin.h
 * 
 * \brief Pin Driver implementation for STM32F446
 * 
 * Provides Pin classes that follow the `cPin` interfaces
 * defined in `<fiber/Interfaces/cPin.hpp>`
 * 
 * ---
 * 
 * Depends on `<stm32f446xx.h>`.
 * 
 * You can provide the dependency to `fiber` in CMake like so:
 * 
 * ```cmake
 * set(CORE_INC_DIRS
 *     Core/Inc
 *     Drivers/CMSIS/Include
 *     Drivers/CMSIS/Device/ST/STM32F4xx/Include
 *     Drivers/STM32F4xx_HAL_Driver/Inc
 *     Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
 * )
 * 
 * target_include_directories(fiber PUBLIC ${CORE_INC_DIRS})
 * ```
 * 
 * ---
 * 
 * Tip: only use micro controller specific drivers when initialising
 * the microcontroller and board. In your application and other drivers
 * only work with concepts (e.g. `<fiber/Interfaces/c*.hpp>`).
 * This will make your application portable
 */

// stm32 HAL
extern "C"{
    #include <stm32f446xx.h> 
}

// fiber
#include <fiber/Interfaces/cPin.hpp>

namespace fiber
{
    namespace stm32f446{

        inline bool GPIO_read_pin(GPIO_TypeDef* GPIOx, uint32_t pin_mask){
            const uint32_t reg = GPIOx->IDR;
            const bool result = (reg & pin_mask) != 0;
            return result;
        }

        inline void GPIO_set_pin_high(GPIO_TypeDef* GPIOx, uint32_t pin_mask){
            GPIOx->BSRR = pin_mask;
        }

        inline void GPIO_set_pin_low(GPIO_TypeDef* GPIOx, uint32_t pin_mask){
            GPIOx->BSRR = (pin_mask << 16);
        }

        inline void GPIO_config_pin_output(GPIO_TypeDef* GPIOx, uint32_t pin_mask){
            // read current config
            uint32_t reg = GPIOx->MODER;

            // calculate the position of the corresponding pin bit pattern
            const uint32_t pos = pin_mask * pin_mask;

            // clear section
            reg &= ~((pos << 1) | pos);

            // set to output
            reg |= (pos);

            // write to register
            GPIOx->MODER = reg;
        }

        inline void GPIO_config_pin_input(GPIO_TypeDef* GPIOx, uint32_t pin_mask){
            // read current config
            uint32_t reg = GPIOx->MODER;

            // calculate the position of the corresponding pin bit pattern
            const uint32_t pos = pin_mask * pin_mask;

            // clear section (cleared is automatically input)
            reg &= ~((pos << 1) | pos);

            // write to register
            GPIOx->MODER = reg;
        }

        class Pin{
        private:

            GPIO_TypeDef* _GPIOx;
            uint32_t _GPIO_Pin_mask;

        constexpr Pin(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin_mask)
            : _GPIOx(GPIOx)
            , _GPIO_Pin_mask(GPIO_Pin_mask)
            {}

        public:

            static constexpr Pin from_pin_mask(GPIO_TypeDef* GPIOx, uint16_t pin_mask){return Pin(GPIOx, pin_mask);}

            static constexpr Pin from_pin_number(GPIO_TypeDef* GPIOx, unsigned int pin_number){return Pin(GPIOx, 1 << pin_number);}

            constexpr Pin(const Pin& other) = default;
            
            constexpr Pin& operator=(const Pin& other) = default;

            inline void high(){
                GPIO_set_pin_high(this->_GPIOx, this->_GPIO_Pin_mask);
            }

            inline void low(){
                GPIO_set_pin_low(this->_GPIOx, this->_GPIO_Pin_mask);
            }

            inline void write(bool level){
                if(level){
                    this->high();
                }else{
                    this->low();
                }
            }

            inline void dir(bool value){
                if(value){
                    this->input();
                }else{
                    this->output();
                }
            }

            inline void input(){
                GPIO_config_pin_input(this->_GPIOx, this->_GPIO_Pin_mask);
            }

            inline void output(){
                GPIO_config_pin_output(this->_GPIOx, this->_GPIO_Pin_mask);
            }

            inline bool read(){
                return GPIO_read_pin(_GPIOx, _GPIO_Pin_mask);
            }

            inline bool is_high(){
                return this->read();
            }

            inline bool is_low(){
                return !this->read();
            }

        }; static_assert(fiber::cPin<Pin>, "Pin needs to satisfy cPin concept");

    } // namespace stm32f446
    

} // namespace fiber
