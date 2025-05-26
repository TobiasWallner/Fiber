#pragma once

#include <array>
#include <cstdint>

/**
 * 
 * Reference Manual: https://www.st.com/resource/en/reference_manual/rm0390-stm32f446xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 * 
 */

namespace fiber{

    namespace stm32f4{

        template<std::unsigned_integral ValueType, class ElementType, size_t BitsPerElement, size_t N>
        inline void array_register_write(volatile ValueType& reg, const std::array<ElementType, N>& values){
            ValueType temp = 0;
            for(unsigned int i = 0; i < 32; ++i){
                temp |= (static_cast<ValueType>(mode[i]) << (i*BitsPerElement));
            }
            this->MODER = temp;
        }

        template<std::unsigned_integral ValueType, class ElementType, size_t BitsPerElement, size_t N>
        inline void array_register_write(volatile ValueType& reg, const ElementType& value, unsigned int index){
            // read register (trying to minimise the number of volatile memory accesses)
            const ValueType register_value = this->MODER;
            
            // clear the bits that will be written
            const ValueType mask = ((1 << BitsPerElement) - 1) << (index*BitsPerElement);
            const ValueType register_value_cleared_mask = register_value & mask;
            
            // generate the new bits where they should be written
            const ValueType integer_value = static_cast<ValueType>(value);
            const ValueType integer_value_to_add = integer_value << (index*BitsPerElement);
            
            // assemble the new register value
            const ValueType result = register_value_cleared_mask | integer_value_to_add;

            // write to the memory (trying to minimise the number of volatile memory accesses)
            this->reg = result;
        }

        template<std::unsigned_integral ValueType, class ElementType, size_t BitsPerElement, size_t N>
        inline std::array<ElementType, N> array_register_read(volatile ValueType& reg){
            std::array<eMode, 32> result;
            const ValueType temp = this->MODER;
            const mask = (1 << bits_per_element) - 1;
            for(unsigned int i = 0; i < 32; ++i){
                result[i] = static_cast<ElementType>((temp >> i*BitsPerElement) & mask);
            }
            return result;
        }

        template<std::unsigned_integral ValueType, class ElementType, size_t BitsPerElement, size_t N>
        inline ElementType array_register_read(volatile ValueType& regptr, unsigned int index){
            // read register (trying to minimise the number of volatile memory accesses)
            const ValueType register_value = this->MODER;

            // shift down so that the relevant bits are the LSB position
            const ValueType shifted_down = (register_value >> (index*BitsPerElement));

            // mask only relevant bits
            const ValueType mask = ((1 << BitsPerElement) - 1);
            const ValueType integer_result = shifted_down & mask;

            // cast to the proper result type
            const ElementType result = static_cast<ElementType>(integer_result);

            return result;
        }

        template<std::unsigned_integral ValueType, class ElementType, size_t BitsPerElement, size_t N>
        class ArrayRegister{
            private:
            volatile ValueType reg;

            public:

            using value_type = ValueType;
            using element_type = ElementType;
            using bits_per_element = BitsPerElement;
            using num = N;

            inline void write(const std::array<ElementType, N>& values){
                array_register_write<ValueType, ElementType, BitsPerElement, N>(values);
            }

            inline void write(const ElementType& value, unsigned int index){
                array_register_write<ValueType, ElementType, BitsPerElement, N>(values, index);
            }

            inline std::array<ElementType, N> read(){
                return array_register_read<ValueType, ElementType, BitsPerElement, N>();
            }

            inline ElementType read(unsigned int i){
                return array_register_read<ValueType, ElementType, BitsPerElement, N>(index);
            }
        };
        
        template<std::unsigned_integral ValueType, class ElementType, size_t BitsPerElement, size_t N>
        class ReadOnlyArrayRegister{
            private:
            volatile ValueType reg;

            public:
            using value_type = ValueType;
            using element_type = ElementType;
            using bits_per_element = BitsPerElement;
            using num = N;

            inline std::array<ElementType, N> read(){
                return array_register_read<ValueType, ElementType, BitsPerElement, N>();
            }

            inline ElementType read(unsigned int i){
                return array_register_read<ValueType, ElementType, BitsPerElement, N>(index);
            }
        };

        struct GPIO{
            // types/enums for settings
            enum class eMode{
                Input = 0b00,
                Output = 0b01,
                AlternateFunction = 0b10,
                Analog = 0b11
            };

            enum class eOutputType{
                PushPull = 0,
                OpenDrain = 1
            };

            enum class eOutputSpeed{
                LowSpeed = 0b00,
                MediumSpeed = 0b01,
                FastSpeed = 0b10,
                HighSpeed = 0b11
            };

            enum class ePullUpDown{
                None = 0b00,
                PullUp = 0b01,
                PullDown = 0b10,
            }

            // registers
            ArrayRegister<uint32_t, eMode, 2, 32> MODER;
            ArrayRegister<uint32_t, eOutputType, 1, 32> OTYPER;
            ArrayRegister<uint32_t, eOutputSpeed, 2, 32> OSPEEDR;
            ArrayRegister<uint32_t, ePullUpDown, 2, 32> PUPDR;
            volatile uint32_t IDR;
            volatile uint32_t ODR;
            volatile uint32_t BSRR;
            volatile uint32_t LCKR;
            volatile uint32_t AFR[2];

            // helper methods
            inline std::array<eMode, 32> mode() const {return this->MODER.read();}
            inline void mode(std::array<eMode, 32> mode){this->MODER.set(mode);}

            inline std::array<eOutputType, 32> output_type() const {return this->OTYPER.read();}
            inline void output_type(std::array<eOutputType, 32> mode){this->OTYPER.set(mode);}

            inline std::array<eOutputSpeed, 32> output_speed() const {return this->OSPEEDR.read();}
            inline void output_speed(std::array<eOutputSpeed, 32> mode){this->OSPEEDR.set(mode);}

            inline std::array<ePullUpDown, 32> pull_up_down() const {return this->PUPDR.read();}
            inline void pull_up_down(std::array<ePullUpDown, 32> mode){this->PUPDR.set(mode);}

        };

        static_assert(sizeof(GPIO) == sizeof(uint32_t) * 10);

    }//namespace stm32f4

}// namespace fiber