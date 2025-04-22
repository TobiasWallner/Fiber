#pragma once

#include <cstddef>
#include <type_traits>
#include <limits>

namespace embed
{

    class UInt8;
    class UInt16;
    class UInt32;
    class UInt64;
    
    class SInt8;
    class SInt16;
    class SInt32;
    class SInt64;
    
    class UInt8{
    private:
        uint8_t _value;
    public:
        constexpr UInt8() = default;
        constexpr UInt8(const UInt8&) = default;
        constexpr UInt8(uint8_t value) noexcept : _value(value){}
        constexpr UInt8(bool value) noexcept : _value((value) ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0)){}
    
        constexpr UInt8& operator=(const UInt8&) = default;
        constexpr UInt8& operator=(uint8_t value) noexcept {this->_value = value; return *this;}
        constexpr UInt8& operator=(bool value) noexcept {this->_value = (value) ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0); return *this;}

        template<std::integral T> explicit constexpr operator T () const noexcept {return static_cast<T>(this->_value);}
        uint8_t value() const noexcept {return this->_value;}
    
        explicit constexpr operator SInt8();

        constexpr void operator += (UInt8 other) noexcept {this->_value += other._value;}
        constexpr void operator -= (UInt8 other) noexcept {this->_value -= other._value;}
        constexpr void operator *= (UInt8 other) noexcept {this->_value *= other._value;}
        constexpr void operator /= (UInt8 other) noexcept {this->_value /= other._value;}
        constexpr void operator %= (UInt8 other) noexcept {this->_value %= other._value;}

        [[nodiscard]]constexpr UInt8 operator + () noexcept const {return *this;}
        [[nodiscard]]constexpr UInt8 operator - () noexcept const {return UInt8(static_cast<uint8_t>(-this->_value));}
    
        constexpr void operator <<= (UInt8 other) noexcept {this->_value <<= other._value;}
        constexpr void operator >>= (UInt8 other) noexcept {this->_value >>= other._value;}
    
        constexpr void operator &= (UInt8 other) noexcept {this->_value &= other._value;}
        constexpr void operator |= (UInt8 other) noexcept {this->_value |= other._value;}
        constexpr void operator ^= (UInt8 other) noexcept {this->_value ^= other._value;}
    
        [[nodiscard]]friend constexpr UInt8 operator + (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value + rhs._value));}
        [[nodiscard]]friend constexpr UInt8 operator - (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value - rhs._value));}
        [[nodiscard]]friend constexpr UInt8 operator * (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value * rhs._value));}
        [[nodiscard]]friend constexpr UInt8 operator / (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value / rhs._value));}
        [[nodiscard]]friend constexpr UInt8 operator % (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value % rhs._value));}

        [[nodiscard]]friend constexpr UInt8 operator << (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value << rhs._value));}
        [[nodiscard]]friend constexpr UInt8 operator >> (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value >> rhs._value));}
    
        [[nodiscard]]friend constexpr UInt8 operator & (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value & rhs._value));}
        [[nodiscard]]friend constexpr UInt8 operator | (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value | rhs._value));}
        [[nodiscard]]friend constexpr UInt8 operator ^ (UInt8 lhs, UInt8 rhs) noexcept {return UInt8(static_cast<uint8_t>(lhs._value ^ rhs._value));}

        [[nodiscard]]friend constexpr bool operator == (UInt8 lhs, UInt8 rhs) noexcept {return lhs._value == rhs._value;}
        [[nodiscard]]friend constexpr bool operator != (UInt8 lhs, UInt8 rhs) noexcept {return lhs._value != rhs._value;}
        [[nodiscard]]friend constexpr bool operator <= (UInt8 lhs, UInt8 rhs) noexcept {return lhs._value <= rhs._value;}
        [[nodiscard]]friend constexpr bool operator >= (UInt8 lhs, UInt8 rhs) noexcept {return lhs._value >= rhs._value;}
        [[nodiscard]]friend constexpr bool operator <  (UInt8 lhs, UInt8 rhs) noexcept {return lhs._value <  rhs._value;}
        [[nodiscard]]friend constexpr bool operator >  (UInt8 lhs, UInt8 rhs) noexcept {return lhs._value >  rhs._value;}
    };
    static_assert(sizeof(UInt8) == 1, "UInt8 must remain 1 byte");
    constexpr UInt8 operator"" ui8(unsigned long long val) {return UInt8(static_cast<uint8_t>(val));}


    class UInt16{
        private:
            uint16_t _value;
        public:
            constexpr UInt16() = default;
            constexpr UInt16(const UInt16&) = default;
            constexpr UInt16(const UInt8& value) : _value(static_cast<uint16_t>value.(value()));
            constexpr UInt16(uint16_t value) noexcept : _value(value){}
            constexpr UInt16(uint8_t value) noexcept : _value(static_cast<uint16_t>(value)){}
            constexpr UInt16(bool value) noexcept : _value((value) ? static_cast<uint16_t>(1) : static_cast<uint16_t>(0)){}
        
            constexpr UInt16& operator=(const UInt16&) = default;
            constexpr UInt16& operator=(const UInt8& value){this->_value = static_cast<uint16_t>(value.value()); return *this;}
            constexpr UInt16& operator=(uint16_t value) noexcept {this->_value = value; return *this;}
            constexpr UInt16& operator=(uint8_t value) noexcept {this->_value = static_cast<uint16_t>(value); return *this;}
            constexpr UInt16& operator=(bool value) noexcept {this->_value = (value) ? static_cast<uint16_t>(1) : static_cast<uint16_t>(0); return *this;}
    
            template<std::integral T> explicit constexpr operator T () const noexcept {return static_cast<T>(this->_value);}
            uint16_t value() const noexcept {return this->_value;}

            explicit constexpr operator UInt8();
            explicit constexpr operator SInt8();
            explicit constexpr operator SInt16();

            constexpr void operator += (UInt16 other) noexcept {this->_value += other._value;}
            constexpr void operator -= (UInt16 other) noexcept {this->_value -= other._value;}
            constexpr void operator *= (UInt16 other) noexcept {this->_value *= other._value;}
            constexpr void operator /= (UInt16 other) noexcept {this->_value /= other._value;}
            constexpr void operator %= (UInt16 other) noexcept {this->_value %= other._value;}
    
            [[nodiscard]]constexpr UInt16 operator + () noexcept const {return *this;}
            [[nodiscard]]constexpr UInt16 operator - () noexcept const {return UInt16(stati_cast<uint16_t>(-this->_value));}
        
            constexpr void operator <<= (UInt16 other) noexcept {this->_value <<= other._value;}
            constexpr void operator >>= (UInt16 other) noexcept {this->_value >>= other._value;}
        
            constexpr void operator &= (UInt16 other) noexcept {this->_value &= other._value;}
            constexpr void operator |= (UInt16 other) noexcept {this->_value |= other._value;}
            constexpr void operator ^= (UInt16 other) noexcept {this->_value ^= other._value;}
        
            [[nodiscard]]friend constexpr UInt16 operator + (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value + rhs._value));}
            [[nodiscard]]friend constexpr UInt16 operator - (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value - rhs._value));}
            [[nodiscard]]friend constexpr UInt16 operator * (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value * rhs._value));}
            [[nodiscard]]friend constexpr UInt16 operator / (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value / rhs._value));}
            [[nodiscard]]friend constexpr UInt16 operator % (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value % rhs._value));}
    
            [[nodiscard]]friend constexpr UInt16 operator << (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value << rhs._value));}
            [[nodiscard]]friend constexpr UInt16 operator >> (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value >> rhs._value));}
        
            [[nodiscard]]friend constexpr UInt16 operator & (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value & rhs._value));}
            [[nodiscard]]friend constexpr UInt16 operator | (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value | rhs._value));}
            [[nodiscard]]friend constexpr UInt16 operator ^ (UInt16 lhs, UInt16 rhs) noexcept {return UInt16(static_cast<uint16_t>(lhs._value ^ rhs._value));}
    
            [[nodiscard]]friend constexpr bool operator == (UInt16 lhs, UInt16 rhs) noexcept {return lhs._value == rhs._value;}
            [[nodiscard]]friend constexpr bool operator != (UInt16 lhs, UInt16 rhs) noexcept {return lhs._value != rhs._value;}
            [[nodiscard]]friend constexpr bool operator <= (UInt16 lhs, UInt16 rhs) noexcept {return lhs._value <= rhs._value;}
            [[nodiscard]]friend constexpr bool operator >= (UInt16 lhs, UInt16 rhs) noexcept {return lhs._value >= rhs._value;}
            [[nodiscard]]friend constexpr bool operator <  (UInt16 lhs, UInt16 rhs) noexcept {return lhs._value <  rhs._value;}
            [[nodiscard]]friend constexpr bool operator >  (UInt16 lhs, UInt16 rhs) noexcept {return lhs._value >  rhs._value;}
        };
        static_assert(sizeof(UInt16) == 2, "UInt16 must remain 2 bytes");
        constexpr UInt16 operator"" ui16(unsigned long long val) {return UInt16(static_cast<uint16_t>(val));}



        class UInt32{
            private:
                uint32_t _value;
            public:
                constexpr UInt32() = default;
                constexpr UInt32(const UInt32&) = default;
                constexpr UInt32(const UInt16& value) : _value(static_cast<uint32_t>value.(value()));
                constexpr UInt32(const UInt8& value) : _value(static_cast<uint32_t>value.(value()));
                constexpr UInt32(uint32_t value) noexcept : _value(value){}
                constexpr UInt32(uint16_t value) noexcept : _value(static_cast<uint32_t>(value)){}
                constexpr UInt32(uint8_t value) noexcept : _value(static_cast<uint32_t>(value)){}
                constexpr UInt32(bool value) noexcept : _value((value) ? static_cast<uint32_t>(1) : static_cast<uint32_t>(0)){}
            
                constexpr UInt32& operator=(const UInt32&) = default;
                constexpr UInt32& operator=(const UInt16& value){this->_value = static_cast<uint32_t>(value.value()); return *this;}
                constexpr UInt32& operator=(const UInt8& value){this->_value = static_cast<uint32_t>(value.value()); return *this;}
                constexpr UInt32& operator=(uint32_t value) noexcept {this->_value = value; return *this;}
                constexpr UInt32& operator=(uint16_t value) noexcept {this->_value = static_cast<uint32_t>(value); return *this;}
                constexpr UInt32& operator=(uint8_t value) noexcept {this->_value = static_cast<uint32_t>(value); return *this;}
                constexpr UInt32& operator=(bool value) noexcept {this->_value = (value) ? static_cast<uint32_t>(1) : static_cast<uint32_t>(0); return *this;}
        
                template<std::integral T> explicit constexpr operator T () const noexcept {return static_cast<T>(this->_value);}
                uint32_t value() const noexcept {return this->_value;}
            
                explicit constexpr operator UInt8();
                explicit constexpr operator UInt16();

                explicit constexpr operator SInt8();
                explicit constexpr operator SInt16();
                explicit constexpr operator SInt32();
                

                constexpr void operator += (UInt32 other) noexcept {this->_value += other._value;}
                constexpr void operator -= (UInt32 other) noexcept {this->_value -= other._value;}
                constexpr void operator *= (UInt32 other) noexcept {this->_value *= other._value;}
                constexpr void operator /= (UInt32 other) noexcept {this->_value /= other._value;}
                constexpr void operator %= (UInt32 other) noexcept {this->_value %= other._value;}
        
                [[nodiscard]]constexpr UInt32 operator + () noexcept const {return *this;}
                [[nodiscard]]constexpr UInt32 operator - () noexcept const {return UInt32(static_cast<uint32_t>(-this->_value));}
            
                constexpr void operator <<= (UInt32 other) noexcept {this->_value <<= other._value;}
                constexpr void operator >>= (UInt32 other) noexcept {this->_value >>= other._value;}
            
                constexpr void operator &= (UInt32 other) noexcept {this->_value &= other._value;}
                constexpr void operator |= (UInt32 other) noexcept {this->_value |= other._value;}
                constexpr void operator ^= (UInt32 other) noexcept {this->_value ^= other._value;}
            
                [[nodiscard]]friend constexpr UInt32 operator + (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value + rhs._value));}
                [[nodiscard]]friend constexpr UInt32 operator - (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value - rhs._value));}
                [[nodiscard]]friend constexpr UInt32 operator * (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value * rhs._value));}
                [[nodiscard]]friend constexpr UInt32 operator / (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value / rhs._value));}
                [[nodiscard]]friend constexpr UInt32 operator % (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value % rhs._value));}
        
                [[nodiscard]]friend constexpr UInt32 operator << (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value << rhs._value));}
                [[nodiscard]]friend constexpr UInt32 operator >> (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value >> rhs._value));}
            
                [[nodiscard]]friend constexpr UInt32 operator & (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value & rhs._value));}
                [[nodiscard]]friend constexpr UInt32 operator | (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value | rhs._value));}
                [[nodiscard]]friend constexpr UInt32 operator ^ (UInt32 lhs, UInt32 rhs) noexcept {return UInt32(static_cast<uint16_t>(lhs._value ^ rhs._value));}
        
                [[nodiscard]]friend constexpr bool operator == (UInt32 lhs, UInt32 rhs) noexcept {return lhs._value == rhs._value;}
                [[nodiscard]]friend constexpr bool operator != (UInt32 lhs, UInt32 rhs) noexcept {return lhs._value != rhs._value;}
                [[nodiscard]]friend constexpr bool operator <= (UInt32 lhs, UInt32 rhs) noexcept {return lhs._value <= rhs._value;}
                [[nodiscard]]friend constexpr bool operator >= (UInt32 lhs, UInt32 rhs) noexcept {return lhs._value >= rhs._value;}
                [[nodiscard]]friend constexpr bool operator <  (UInt32 lhs, UInt32 rhs) noexcept {return lhs._value <  rhs._value;}
                [[nodiscard]]friend constexpr bool operator >  (UInt32 lhs, UInt32 rhs) noexcept {return lhs._value >  rhs._value;}
            };
            static_assert(sizeof(UInt32) == 4, "UInt32 must remain 4 bytes");
            constexpr UInt32 operator"" ui32(unsigned long long val) {return UInt32(static_cast<uint32_t>(val));}


    class UInt64{
        private:
            uint64_t _value;
        public:
            constexpr UInt64() = default;
            constexpr UInt64(const UInt64&) = default;
            constexpr UInt64(const UInt32& value) : _value(static_cast<uint64_t>value.(value()));
            constexpr UInt64(const UInt16& value) : _value(static_cast<uint64_t>value.(value()));
            constexpr UInt64(const UInt8& value) : _value(static_cast<uint64_t>value.(value()));
            constexpr UInt64(uint64_t value) noexcept : _value(value){}
            constexpr UInt64(uint32_t value) noexcept : _value(static_cast<uint64_t>(value)){}
            constexpr UInt64(uint16_t value) noexcept : _value(static_cast<uint64_t>(value)){}
            constexpr UInt64(uint8_t value) noexcept : _value(static_cast<uint64_t>(value)){}
            constexpr UInt64(bool value) noexcept : _value((value) ? static_cast<uint64_t>(1) : static_cast<uint64_t>(0)){}
        
            constexpr UInt64& operator=(const UInt64&) = default;
            constexpr UInt64& operator=(const UInt32& value){this->_value = static_cast<uint64_t>(value.value()); return *this;}
            constexpr UInt64& operator=(const UInt16& value){this->_value = static_cast<uint64_t>(value.value()); return *this;}
            constexpr UInt64& operator=(const UInt8& value){this->_value = static_cast<uint64_t>(value.value()); return *this;}
            constexpr UInt64& operator=(uint64_t value) noexcept {this->_value = value; return *this;}
            constexpr UInt64& operator=(uint32_t value) noexcept {this->_value = static_cast<uint64_t>(value); return *this;}
            constexpr UInt64& operator=(uint16_t value) noexcept {this->_value = static_cast<uint64_t>(value); return *this;}
            constexpr UInt64& operator=(uint8_t value) noexcept {this->_value = static_cast<uint64_t>(value); return *this;}
            constexpr UInt64& operator=(bool value) noexcept {this->_value = (value) ? static_cast<uint64_t>(1) : static_cast<uint64_t>(0); return *this;}
    
            template<std::integral T> explicit constexpr operator T () const noexcept {return static_cast<T>(this->_value);}
            uint64_t value() const noexcept {return this->_value;}

            explicit constexpr operator UInt8();
            explicit constexpr operator UInt16();
            explicit constexpr operator UInt32();

            explicit constexpr operator SInt8();
            explicit constexpr operator SInt16();
            explicit constexpr operator SInt32();
            explicit constexpr operator SInt64();
        
            constexpr void operator += (UInt64 other) noexcept {this->_value += other._value;}
            constexpr void operator -= (UInt64 other) noexcept {this->_value -= other._value;}
            constexpr void operator *= (UInt64 other) noexcept {this->_value *= other._value;}
            constexpr void operator /= (UInt64 other) noexcept {this->_value /= other._value;}
            constexpr void operator %= (UInt64 other) noexcept {this->_value %= other._value;}
    
            [[nodiscard]]constexpr UInt64 operator + () noexcept const {return *this;}
            [[nodiscard]]constexpr UInt64 operator - () noexcept const {return UInt64(static_cast<uint64_t>(-this->_value));}
        
            constexpr void operator <<= (UInt64 other) noexcept {this->_value <<= other._value;}
            constexpr void operator >>= (UInt64 other) noexcept {this->_value >>= other._value;}
        
            constexpr void operator &= (UInt64 other) noexcept {this->_value &= other._value;}
            constexpr void operator |= (UInt64 other) noexcept {this->_value |= other._value;}
            constexpr void operator ^= (UInt64 other) noexcept {this->_value ^= other._value;}
        
            [[nodiscard]]friend constexpr UInt64 operator + (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value + rhs._value));}
            [[nodiscard]]friend constexpr UInt64 operator - (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value - rhs._value));}
            [[nodiscard]]friend constexpr UInt64 operator * (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value * rhs._value));}
            [[nodiscard]]friend constexpr UInt64 operator / (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value / rhs._value));}
            [[nodiscard]]friend constexpr UInt64 operator % (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value % rhs._value));}
    
            [[nodiscard]]friend constexpr UInt64 operator << (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value << rhs._value));}
            [[nodiscard]]friend constexpr UInt64 operator >> (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value >> rhs._value));}
        
            [[nodiscard]]friend constexpr UInt64 operator & (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value & rhs._value));}
            [[nodiscard]]friend constexpr UInt64 operator | (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value | rhs._value));}
            [[nodiscard]]friend constexpr UInt64 operator ^ (UInt64 lhs, UInt64 rhs) noexcept {return UInt64(static_cast<uint16_t>(lhs._value ^ rhs._value));}
    
            [[nodiscard]]friend constexpr bool operator == (UInt64 lhs, UInt64 rhs) noexcept {return lhs._value == rhs._value;}
            [[nodiscard]]friend constexpr bool operator != (UInt64 lhs, UInt64 rhs) noexcept {return lhs._value != rhs._value;}
            [[nodiscard]]friend constexpr bool operator <= (UInt64 lhs, UInt64 rhs) noexcept {return lhs._value <= rhs._value;}
            [[nodiscard]]friend constexpr bool operator >= (UInt64 lhs, UInt64 rhs) noexcept {return lhs._value >= rhs._value;}
            [[nodiscard]]friend constexpr bool operator <  (UInt64 lhs, UInt64 rhs) noexcept {return lhs._value <  rhs._value;}
            [[nodiscard]]friend constexpr bool operator >  (UInt64 lhs, UInt64 rhs) noexcept {return lhs._value >  rhs._value;}
    };
    static_assert(sizeof(UInt64) == 8, "UInt64 must remain 8 bytes");
    constexpr UInt64 operator"" ui32(unsigned long long val) {return UInt32(static_cast<uint64_t>(val));}


    class SInt8{
        private:
            int8_t _value;
        public:
            constexpr SInt8() = default;
            constexpr SInt8(const SInt8&) = default;
            constexpr SInt8(int8_t value) noexcept : _value(value){}
            constexpr SInt8(bool value) noexcept : _value((value) ? static_cast<int8_t>(1) : static_cast<int8_t>(0)){}
        
            constexpr SInt8& operator=(const Sint8&) = default;
            constexpr SInt8& operator=(int8_t value) noexcept {this->_value = value; return *this;}
            constexpr SInt8& operator=(bool value) noexcept {this->_value = (value) ? static_cast<int8_t>(1) : static_cast<int8_t>(0); return *this;}

            template<std::integral T> explicit constexpr operator T () const noexcept {return static_cast<T>(this->_value);}
            int8_t value() const noexcept {return this->_value;}

            explicit constexpr SInt8::operator UInt8() const noexcept ;
            explicit constexpr SInt8::operator UInt16() const noexcept;
            explicit constexpr SInt8::operator UInt32() const noexcept;
            explicit constexpr SInt8::operator UInt64() const noexcept;
        
            constexpr void operator += (SInt8 other) noexcept {this->_value += other._value;}
            constexpr void operator -= (SInt8 other) noexcept {this->_value -= other._value;}
            constexpr void operator *= (SInt8 other) noexcept {this->_value *= other._value;}
            constexpr void operator /= (SInt8 other) noexcept {this->_value /= other._value;}
            constexpr void operator %= (SInt8 other) noexcept {this->_value %= other._value;}

            [[nodiscard]]constexpr SInt8 operator + () noexcept const {return *this;}
            [[nodiscard]]constexpr SInt8 operator - () noexcept const {return SInt8(static_cast<int8_t>(-this->_value));}
        
            constexpr void operator <<= (SInt8 other) noexcept {this->_value <<= other._value;}
            constexpr void operator >>= (SInt8 other) noexcept {this->_value >>= other._value;}
        
            constexpr void operator &= (SInt8 other) noexcept {this->_value &= other._value;}
            constexpr void operator |= (SInt8 other) noexcept {this->_value |= other._value;}
            constexpr void operator ^= (SInt8 other) noexcept {this->_value ^= other._value;}
        
            [[nodiscard]]friend constexpr SInt8 operator + (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value + rhs._value));}
            [[nodiscard]]friend constexpr SInt8 operator - (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value - rhs._value));}
            [[nodiscard]]friend constexpr SInt8 operator * (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value * rhs._value));}
            [[nodiscard]]friend constexpr SInt8 operator / (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value / rhs._value));}
            [[nodiscard]]friend constexpr SInt8 operator % (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value % rhs._value));}

            [[nodiscard]]friend constexpr SInt8 operator << (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value << rhs._value));}
            [[nodiscard]]friend constexpr SInt8 operator >> (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value >> rhs._value));}
        
            [[nodiscard]]friend constexpr SInt8 operator & (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value & rhs._value));}
            [[nodiscard]]friend constexpr SInt8 operator | (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value | rhs._value));}
            [[nodiscard]]friend constexpr SInt8 operator ^ (SInt8 lhs, SInt8 rhs) noexcept {return SInt8(static_cast<int8_t>(lhs._value ^ rhs._value));}

            [[nodiscard]]friend constexpr bool operator == (SInt8 lhs, SInt8 rhs) noexcept {return lhs._value == rhs._value;}
            [[nodiscard]]friend constexpr bool operator != (SInt8 lhs, SInt8 rhs) noexcept {return lhs._value != rhs._value;}
            [[nodiscard]]friend constexpr bool operator <= (SInt8 lhs, SInt8 rhs) noexcept {return lhs._value <= rhs._value;}
            [[nodiscard]]friend constexpr bool operator >= (SInt8 lhs, SInt8 rhs) noexcept {return lhs._value >= rhs._value;}
            [[nodiscard]]friend constexpr bool operator <  (SInt8 lhs, SInt8 rhs) noexcept {return lhs._value <  rhs._value;}
            [[nodiscard]]friend constexpr bool operator >  (SInt8 lhs, SInt8 rhs) noexcept {return lhs._value >  rhs._value;}
    };
    static_assert(sizeof(SInt8) == 1, "SInt8 must remain 1 byte");
    constexpr SInt8 operator"" si8(unsigned long long val) {return SInt8(static_cast<int8_t>(val));}
    
    class SInt16{
        private:
            int16_t _value;
        public:
            constexpr SInt16() = default;
            constexpr SInt16(const SInt16&) = default;
            constexpr SInt16(const SInt8& value) noexcept : _value(static_cast<int16_t>(value));
            constexpr SInt16(const UInt8& value) noexcept : _value(static_cast<int16_t>(value));
            constexpr SInt16(int16_t value) noexcept : _value(static_cast<int16_t>(value)){}
            constexpr SInt16(int8_t value) noexcept : _value(static_cast<int16_t>(value)){}
            constexpr SInt16(uint8_t value) noexcept : _value(static_cast<int16_t>(value)){}
            constexpr SInt16(bool value) noexcept : _value((value) ? static_cast<int16_t>(1) : static_cast<int16_t>(0)){}
        
            constexpr SInt16& operator=(const SInt16&) = default;
            constexpr SInt16& operator=(const SInt8& value) noexcept {this->_value = static_cast<int16_t>(value); return *this;}
            constexpr SInt16& operator=(const UInt8& value) noexcept {this->_value = static_cast<int16_t>(value); return *this;}
            constexpr SInt16& operator=(int16_t value) noexcept {this->_value = static_cast<int16_t>(value); return *this;}
            constexpr SInt16& operator=(int8_t value) noexcept {this->_value = static_cast<int16_t>(value); return *this;}
            constexpr SInt16& operator=(uint8_t value) noexcept {this->_value = static_cast<int16_t>(value); return *this;}
            constexpr SInt16& operator=(bool value) noexcept {this->_value = ((value) ? static_cast<int16_t>(1) : static_cast<int16_t>(0)); return *this;}

            template<std::integral T> explicit constexpr operator T () const noexcept {return static_cast<T>(this->_value);}
            int8_t value() const noexcept {return this->_value;}

            explicit constexpr SInt16::operator UInt8() const noexcept ;
            explicit constexpr SInt16::operator UInt16() const noexcept;
            explicit constexpr SInt16::operator UInt32() const noexcept;
            explicit constexpr SInt16::operator UInt64() const noexcept;
            explicit constexpr SInt16::operator SInt8() const noexcept ;
        

            constexpr void operator += (SInt16 other) noexcept {this->_value += other._value;}
            constexpr void operator -= (SInt16 other) noexcept {this->_value -= other._value;}
            constexpr void operator *= (SInt16 other) noexcept {this->_value *= other._value;}
            constexpr void operator /= (SInt16 other) noexcept {this->_value /= other._value;}
            constexpr void operator %= (SInt16 other) noexcept {this->_value %= other._value;}

            [[nodiscard]]constexpr SInt16 operator + () noexcept const {return *this;}
            [[nodiscard]]constexpr SInt16 operator - () noexcept const {return SInt16(static_cast<int16_t>(-this->_value));}
        
            constexpr void operator <<= (SInt16 other) noexcept {this->_value <<= other._value;}
            constexpr void operator >>= (SInt16 other) noexcept {this->_value >>= other._value;}
        
            constexpr void operator &= (SInt16 other) noexcept {this->_value &= other._value;}
            constexpr void operator |= (SInt16 other) noexcept {this->_value |= other._value;}
            constexpr void operator ^= (SInt16 other) noexcept {this->_value ^= other._value;}
        
            [[nodiscard]]friend constexpr SInt16 operator +  (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value +  rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator -  (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value -  rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator *  (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value *  rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator /  (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value /  rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator %  (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value %  rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator << (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value << rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator >> (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value >> rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator &  (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value &  rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator |  (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value |  rhs._value));}
            [[nodiscard]]friend constexpr SInt16 operator ^  (SInt16 lhs, SInt16 rhs) noexcept {return SInt16(static_cast<int16_t>(lhs._value ^  rhs._value));}

            [[nodiscard]]friend constexpr bool operator == (SInt16 lhs, SInt16 rhs) noexcept {return lhs._value == rhs._value;}
            [[nodiscard]]friend constexpr bool operator != (SInt16 lhs, SInt16 rhs) noexcept {return lhs._value != rhs._value;}
            [[nodiscard]]friend constexpr bool operator <= (SInt16 lhs, SInt16 rhs) noexcept {return lhs._value <= rhs._value;}
            [[nodiscard]]friend constexpr bool operator >= (SInt16 lhs, SInt16 rhs) noexcept {return lhs._value >= rhs._value;}
            [[nodiscard]]friend constexpr bool operator <  (SInt16 lhs, SInt16 rhs) noexcept {return lhs._value <  rhs._value;}
            [[nodiscard]]friend constexpr bool operator >  (SInt16 lhs, SInt16 rhs) noexcept {return lhs._value >  rhs._value;}
    };
    static_assert(sizeof(SInt16) == 2, "SInt16 must remain 2 byte");
    constexpr SInt16 operator"" si16(unsigned long long val) {return SInt16(static_cast<int16_t>(val));}


    class SInt32{
        private:
            int32_t _value;
        public:
            constexpr SInt32() = default;
            constexpr SInt32(const SInt32&) = default;
            constexpr SInt32(const SInt16& value) noexcept : _value(static_cast<int32_t>(value));
            constexpr SInt32(const SInt8& value) noexcept : _value(static_cast<int32_t>(value));
            constexpr SInt32(const UInt16& value) noexcept : _value(static_cast<int32_t>(value));
            constexpr SInt32(const UInt8& value) noexcept : _value(static_cast<int32_t>(value));
            constexpr SInt32(int16_t value) noexcept : _value(static_cast<int32_t>(value)){}
            constexpr SInt32(int8_t value) noexcept : _value(static_cast<int32_t>(value)){}
            constexpr SInt32(uint16_t value) noexcept : _value(static_cast<int32_t>(value)){}
            constexpr SInt32(uint8_t value) noexcept : _value(static_cast<int32_t>(value)){}
            constexpr SInt32(bool value) noexcept : _value((value) ? static_cast<int32_t>(1) : static_cast<int32_t>(0)){}
        
            constexpr SInt32& operator=(const SInt32&) = default;
            constexpr SInt32& operator=(const SInt16& value) noexcept {this->_value = static_cast<int32_t>(value); return *this;}
            constexpr SInt32& operator=(const SInt8& value) noexcept {this->_value = static_cast<int32_t>(value); return *this;}
            constexpr SInt32& operator=(const UInt16& value) noexcept {this->_value = static_cast<int32_t>(value); return *this;}
            constexpr SInt32& operator=(const UInt8& value) noexcept {this->_value = static_cast<int32_t>(value); return *this;}
            constexpr SInt32& operator=(int16_t value) noexcept {this->_value = static_cast<int32_t>(value); return *this;}
            constexpr SInt32& operator=(int8_t value) noexcept {this->_value = static_cast<int32_t>(value); return *this;}
            constexpr SInt32& operator=(uint16_t value) noexcept {this->_value = static_cast<int32_t>(value); return *this;}
            constexpr SInt32& operator=(uint8_t value) noexcept {this->_value = static_cast<int32_t>(value); return *this;}
            constexpr SInt32& operator=(bool value) noexcept {this->_value = ((value) ? static_cast<int32_t>(1) : static_cast<int32_t>(0)); return *this;}

            template<std::integral T> explicit constexpr operator T () const noexcept {return static_cast<T>(this->_value);}
            int8_t value() const noexcept {return this->_value;}

            explicit constexpr SInt32::operator UInt8() const noexcept ;
            explicit constexpr SInt32::operator UInt16() const noexcept;
            explicit constexpr SInt32::operator UInt32() const noexcept;
            explicit constexpr SInt32::operator UInt64() const noexcept;
            explicit constexpr SInt32::operator SInt8() const noexcept ;
            explicit constexpr SInt32::operator SInt16() const noexcept;
        
            constexpr void operator += (SInt32 other) noexcept {this->_value += other._value;}
            constexpr void operator -= (SInt32 other) noexcept {this->_value -= other._value;}
            constexpr void operator *= (SInt32 other) noexcept {this->_value *= other._value;}
            constexpr void operator /= (SInt32 other) noexcept {this->_value /= other._value;}
            constexpr void operator %= (SInt32 other) noexcept {this->_value %= other._value;}

            [[nodiscard]]constexpr SInt32 operator + () noexcept const {return *this;}
            [[nodiscard]]constexpr SInt32 operator - () noexcept const {return SInt32(static_cast<int32_t>(-this->_value));}
        
            constexpr void operator <<= (SInt32 other) noexcept {this->_value <<= other._value;}
            constexpr void operator >>= (SInt32 other) noexcept {this->_value >>= other._value;}
        
            constexpr void operator &= (SInt32 other) noexcept {this->_value &= other._value;}
            constexpr void operator |= (SInt32 other) noexcept {this->_value |= other._value;}
            constexpr void operator ^= (SInt32 other) noexcept {this->_value ^= other._value;}
        
            [[nodiscard]]friend constexpr SInt32 operator +  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value + rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator -  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value - rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator *  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value * rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator /  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value / rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator %  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value % rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator << (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value << rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator >> (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value >> rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator &  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value & rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator |  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value | rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator ^  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value ^ rhs._value));}

            [[nodiscard]]friend constexpr bool operator == (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value == rhs._value;}
            [[nodiscard]]friend constexpr bool operator != (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value != rhs._value;}
            [[nodiscard]]friend constexpr bool operator <= (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value <= rhs._value;}
            [[nodiscard]]friend constexpr bool operator >= (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value >= rhs._value;}
            [[nodiscard]]friend constexpr bool operator <  (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value <  rhs._value;}
            [[nodiscard]]friend constexpr bool operator >  (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value >  rhs._value;}
    };
    static_assert(sizeof(SInt32) == 4, "SInt32 must remain 4 byte");
    constexpr SInt32 operator"" si32(unsigned long long val) {return SInt32(static_cast<int32_t>(val));}


    class SInt64{
        private:
            int32_t _value;
        public:
            constexpr SInt64() = default;
            constexpr SInt64(const SInt64&) = default;
            constexpr SInt64(const SInt32& value) noexcept : _value(static_cast<int64_t>(value));
            constexpr SInt64(const SInt16& value) noexcept : _value(static_cast<int64_t>(value));
            constexpr SInt64(const SInt8& value)  noexcept : _value(static_cast<int64_t>(value));
            constexpr SInt64(const UInt16& value) noexcept : _value(static_cast<int64_t>(value));
            constexpr SInt64(const UInt16& value) noexcept : _value(static_cast<int64_t>(value));
            constexpr SInt64(const UInt8& value)  noexcept : _value(static_cast<int64_t>(value));
            constexpr SInt64(int16_t value)  noexcept : _value(static_cast<int64_t>(value)){}
            constexpr SInt64(int8_t value)   noexcept : _value(static_cast<int64_t>(value)){}
            constexpr SInt64(uint16_t value) noexcept : _value(static_cast<int64_t>(value)){}
            constexpr SInt64(uint8_t value)  noexcept : _value(static_cast<int64_t>(value)){}
            constexpr SInt64(bool value)     noexcept : _value((value) ? static_cast<int64_t>(1) : static_cast<int64_t>(0)){}
        
            constexpr SInt64& operator=(const SInt64&) = default;
            constexpr SInt64& operator=(const SInt32& value) noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(const SInt16& value) noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(const SInt8& value)  noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(const UInt16& value) noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(const UInt16& value) noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(const UInt8& value)  noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(int16_t value)  noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(int8_t value)   noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(uint16_t value) noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(uint8_t value)  noexcept {this->_value = static_cast<int64_t>(value);}
            constexpr SInt64& operator=(bool value)     noexcept {this->_value = ((value) ? static_cast<int64_t>(1) : static_cast<int64_t>(0))}

            template<std::integral T> explicit constexpr operator T () const noexcept {return static_cast<T>(this->_value);}
            int8_t value() const noexcept {return this->_value;}

            explicit constexpr SInt64::operator UInt8()  const noexcept;
            explicit constexpr SInt64::operator UInt16() const noexcept;
            explicit constexpr SInt64::operator UInt32() const noexcept;
            explicit constexpr SInt64::operator UInt64() const noexcept;
            explicit constexpr SInt64::operator SInt8()  const noexcept;
            explicit constexpr SInt64::operator SInt16() const noexcept;
            explicit constexpr SInt64::operator SInt32() const noexcept;
        
            constexpr void operator += (SInt32 other) noexcept {this->_value += other._value;}
            constexpr void operator -= (SInt32 other) noexcept {this->_value -= other._value;}
            constexpr void operator *= (SInt32 other) noexcept {this->_value *= other._value;}
            constexpr void operator /= (SInt32 other) noexcept {this->_value /= other._value;}
            constexpr void operator %= (SInt32 other) noexcept {this->_value %= other._value;}

            [[nodiscard]]constexpr SInt32 operator + () noexcept const {return *this;}
            [[nodiscard]]constexpr SInt32 operator - () noexcept const {return SInt32(static_cast<int32_t>(-this->_value));}
        
            constexpr void operator <<= (SInt32 other) noexcept {this->_value <<= other._value;}
            constexpr void operator >>= (SInt32 other) noexcept {this->_value >>= other._value;}
        
            constexpr void operator &= (SInt32 other) noexcept {this->_value &= other._value;}
            constexpr void operator |= (SInt32 other) noexcept {this->_value |= other._value;}
            constexpr void operator ^= (SInt32 other) noexcept {this->_value ^= other._value;}
        
            [[nodiscard]]friend constexpr SInt32 operator +  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value + rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator -  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value - rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator *  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value * rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator /  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value / rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator %  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value % rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator << (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value << rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator >> (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value >> rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator &  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value & rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator |  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value | rhs._value));}
            [[nodiscard]]friend constexpr SInt32 operator ^  (SInt32 lhs, SInt32 rhs) noexcept {return SInt32(static_cast<int32_t>(lhs._value ^ rhs._value));}

            [[nodiscard]]friend constexpr bool operator == (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value == rhs._value;}
            [[nodiscard]]friend constexpr bool operator != (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value != rhs._value;}
            [[nodiscard]]friend constexpr bool operator <= (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value <= rhs._value;}
            [[nodiscard]]friend constexpr bool operator >= (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value >= rhs._value;}
            [[nodiscard]]friend constexpr bool operator <  (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value <  rhs._value;}
            [[nodiscard]]friend constexpr bool operator >  (SInt32 lhs, SInt32 rhs) noexcept {return lhs._value >  rhs._value;}
    };
    static_assert(sizeof(SInt32) == 4, "SInt32 must remain 4 byte");
    constexpr SInt32 operator"" si32(unsigned long long val) {return SInt32(static_cast<int32_t>(val));}

// ----------------------------------------------------------------------------------------------
//                                  Comparisons Implementations
// ----------------------------------------------------------------------------------------------

    [[nodiscard]] constexpr bool operator == (UInt8 lhs, SInt8 rhs) noexcept {return static_cast<SInt16>(lhs) == static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator == (SInt8 lhs, UInt8 rhs) noexcept {return static_cast<SInt16>(lhs) == static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator != (UInt8 lhs, SInt8 rhs) noexcept {return static_cast<SInt16>(lhs) != static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator != (SInt8 lhs, UInt8 rhs) noexcept {return static_cast<SInt16>(lhs) != static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator <= (UInt8 lhs, SInt8 rhs) noexcept {return static_cast<SInt16>(lhs) <= static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator <= (SInt8 lhs, UInt8 rhs) noexcept {return static_cast<SInt16>(lhs) <= static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator <  (UInt8 lhs, SInt8 rhs) noexcept {return static_cast<SInt16>(lhs) <  static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator <  (SInt8 lhs, UInt8 rhs) noexcept {return static_cast<SInt16>(lhs) <  static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator >= (UInt8 lhs, SInt8 rhs) noexcept {return static_cast<SInt16>(lhs) >= static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator >= (SInt8 lhs, UInt8 rhs) noexcept {return static_cast<SInt16>(lhs) >= static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator >  (UInt8 lhs, SInt8 rhs) noexcept {return static_cast<SInt16>(lhs) >  static_cast<SInt16>(rht);}
    [[nodiscard]] constexpr bool operator >  (SInt8 lhs, UInt8 rhs) noexcept {return static_cast<SInt16>(lhs) >  static_cast<SInt16>(rht);}

    [[nodiscard]] constexpr bool operator == (UInt16 lhs, SInt16 rhs) noexcept {return static_cast<SInt32>(lhs) == static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator == (SInt16 lhs, UInt16 rhs) noexcept {return static_cast<SInt32>(lhs) == static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator != (UInt16 lhs, SInt16 rhs) noexcept {return static_cast<SInt32>(lhs) != static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator != (SInt16 lhs, UInt16 rhs) noexcept {return static_cast<SInt32>(lhs) != static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator <= (UInt16 lhs, SInt16 rhs) noexcept {return static_cast<SInt32>(lhs) <= static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator <= (SInt16 lhs, UInt16 rhs) noexcept {return static_cast<SInt32>(lhs) <= static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator < (UInt16 lhs, SInt16 rhs) noexcept  {return static_cast<SInt32>(lhs) <  static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator < (SInt16 lhs, UInt16 rhs) noexcept  {return static_cast<SInt32>(lhs) <  static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator >= (UInt16 lhs, SInt16 rhs) noexcept {return static_cast<SInt32>(lhs) >= static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator >= (SInt16 lhs, UInt16 rhs) noexcept {return static_cast<SInt32>(lhs) >= static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator > (UInt16 lhs, SInt16 rhs) noexcept  {return static_cast<SInt32>(lhs) >  static_cast<SInt32>(rhs);}
    [[nodiscard]] constexpr bool operator > (SInt16 lhs, UInt16 rhs) noexcept  {return static_cast<SInt32>(lhs) >  static_cast<SInt32>(rhs);}

    [[nodiscard]] constexpr bool operator == (UInt32 lhs, SInt32 rhs) noexcept {return (rhs < 0) ? false : (lhs == static_cast<UInt32>(rhs));}
    [[nodiscard]] constexpr bool operator == (SInt32 lhs, UInt32 rhs) noexcept {return (lhs < 0) ? false : (static_cast<UInt32>(lhs) == rhs);}
    [[nodiscard]] constexpr bool operator != (UInt32 lhs, SInt32 rhs) noexcept {return (rhs < 0) ? true  : (lhs != static_cast<UInt32>(rhs));}
    [[nodiscard]] constexpr bool operator != (SInt32 lhs, UInt32 rhs) noexcept {return (lhs < 0) ? true  : (static_cast<UInt32>(lhs) != rhs);}
    [[nodiscard]] constexpr bool operator <= (UInt32 lhs, SInt32 rhs) noexcept {return (rhs < 0) ? false : (lhs <= static_cast<UInt32>(rhs));}
    [[nodiscard]] constexpr bool operator <= (SInt32 lhs, UInt32 rhs) noexcept {return (lhs < 0) ? true  : (static_cast<UInt32>(lhs) <= rhs);}
    [[nodiscard]] constexpr bool operator <  (UInt32 lhs, SInt32 rhs) noexcept {return (rhs < 0) ? false : (lhs <  static_cast<UInt32>(rhs));}
    [[nodiscard]] constexpr bool operator <  (SInt32 lhs, UInt32 rhs) noexcept {return (lhs < 0) ? true  : (static_cast<UInt32>(lhs) <  rhs);}
    [[nodiscard]] constexpr bool operator >= (UInt32 lhs, SInt32 rhs) noexcept {return (rhs < 0) ? true  : (lhs <= static_cast<UInt32>(rhs));}
    [[nodiscard]] constexpr bool operator >= (SInt32 lhs, UInt32 rhs) noexcept {return (lhs < 0) ? false : (static_cast<UInt32>(lhs) <= rhs);}
    [[nodiscard]] constexpr bool operator >  (UInt32 lhs, SInt32 rhs) noexcept {return (rhs < 0) ? true  : (lhs <  static_cast<UInt32>(rhs));}
    [[nodiscard]] constexpr bool operator >  (SInt32 lhs, UInt32 rhs) noexcept {return (lhs < 0) ? false : (static_cast<UInt32>(lhs) <  rhs);}

    [[nodiscard]] constexpr bool operator == (UInt64 lhs, SInt64 rhs) noexcept {return (rhs < 0) ? false : (lhs == static_cast<UInt64>(rhs));}
    [[nodiscard]] constexpr bool operator == (SInt64 lhs, UInt64 rhs) noexcept {return (lhs < 0) ? false : (static_cast<UInt64>(lhs) == rhs);}
    [[nodiscard]] constexpr bool operator != (UInt64 lhs, SInt64 rhs) noexcept {return (rhs < 0) ? true  : (lhs != static_cast<UInt64>(rhs));}
    [[nodiscard]] constexpr bool operator != (SInt64 lhs, UInt64 rhs) noexcept {return (lhs < 0) ? true  : (static_cast<UInt64>(lhs) != rhs);}
    [[nodiscard]] constexpr bool operator <= (UInt64 lhs, SInt64 rhs) noexcept {return (rhs < 0) ? false : (lhs <= static_cast<UInt64>(rhs));}
    [[nodiscard]] constexpr bool operator <= (SInt64 lhs, UInt64 rhs) noexcept {return (lhs < 0) ? true  : (static_cast<UInt64>(lhs) <= rhs);}
    [[nodiscard]] constexpr bool operator <  (UInt64 lhs, SInt64 rhs) noexcept {return (rhs < 0) ? false : (lhs <  static_cast<UInt64>(rhs));}
    [[nodiscard]] constexpr bool operator <  (SInt64 lhs, UInt64 rhs) noexcept {return (lhs < 0) ? true  : (static_cast<UInt64>(lhs) <  rhs);}
    [[nodiscard]] constexpr bool operator >= (UInt64 lhs, SInt64 rhs) noexcept {return (rhs < 0) ? true  : (lhs <= static_cast<UInt64>(rhs));}
    [[nodiscard]] constexpr bool operator >= (SInt64 lhs, UInt64 rhs) noexcept {return (lhs < 0) ? false : (static_cast<UInt64>(lhs) <= rhs);}
    [[nodiscard]] constexpr bool operator >  (UInt64 lhs, SInt64 rhs) noexcept {return (rhs < 0) ? true  : (lhs <  static_cast<UInt64>(rhs));}
    [[nodiscard]] constexpr bool operator >  (SInt64 lhs, UInt64 rhs) noexcept {return (lhs < 0) ? false : (static_cast<UInt64>(lhs) <  rhs);}

// ----------------------------------------------------------------------------------------------
//                                  Cast Implementations
// ----------------------------------------------------------------------------------------------

    explicit constexpr UInt8::operator SInt8(){SInt8(static_cast<int8_t>(this->_value));}

    explicit constexpr UInt16::operator UInt8(){UInt8(static_cast<uint8_t>(this->_value));}
    explicit constexpr UInt16::operator SInt8(){SInt8(static_cast<int8_t>(this->_value));}
    explicit constexpr UInt16::operator SInt16(){SInt16(static_cast<int16_t>(this->_value));}

    explicit constexpr UInt32::operator UInt8(){UInt8(static_cast<uint8_t>(this->_value));}
    explicit constexpr UInt32::operator UInt16(){UInt16(static_cast<uint16_t>(this->_value));}
    explicit constexpr UInt32::operator SInt8(){SInt8(static_cast<int8_t>(this->_value));}
    explicit constexpr UInt32::operator SInt16(){SInt16(static_cast<int16_t>(this->_value));}
    explicit constexpr UInt32::operator SInt32(){SInt32(static_cast<int32_t>(this->_value));}

    explicit constexpr UInt64::operator UInt8(){UInt8(static_cast<uint8_t>(this->_value));}
    explicit constexpr UInt64::operator UInt16(){UInt16(static_cast<uint16_t>(this->_value));}
    explicit constexpr UInt64::operator UInt32(){UInt32(static_cast<uint32_t>(this->_value));}
    explicit constexpr UInt64::operator SInt8(){SInt8(static_cast<int8_t>(this->_value));}
    explicit constexpr UInt64::operator SInt16(){SInt16(static_cast<int16_t>(this->_value));}
    explicit constexpr UInt64::operator SInt32(){SInt32(static_cast<int32_t>(this->_value));}
    explicit constexpr UInt64::operator SInt64(){SInt64(static_cast<int64_t>(this->_value));}


    explicit constexpr SInt8::operator UInt8() const noexcept {return UInt8(static_cast<uint8_t>(this->_value));}
    explicit constexpr SInt8::operator UInt16() const noexcept {return UInt16(static_cast<uint16_t>(this->_value));}
    explicit constexpr SInt8::operator UInt32() const noexcept {return UInt32(static_cast<uint32_t>(this->_value));}
    explicit constexpr SInt8::operator UInt64() const noexcept {return UInt64(static_cast<uint64_t>(this->_value));}

    explicit constexpr SInt16::operator UInt8() const noexcept {return UInt8(static_cast<uint8_t>(this->_value));}
    explicit constexpr SInt16::operator UInt16() const noexcept {return UInt16(static_cast<uint16_t>(this->_value));}
    explicit constexpr SInt16::operator UInt32() const noexcept {return UInt32(static_cast<uint32_t>(this->_value));}
    explicit constexpr SInt16::operator UInt64() const noexcept {return UInt64(static_cast<uint64_t>(this->_value));}
    explicit constexpr SInt16::operator SInt8() const noexcept {return SInt8(static_cast<int8_t>(this->_value));}

    explicit constexpr SInt32::operator UInt8() const noexcept {return UInt8(static_cast<uint8_t>(this->_value));}
    explicit constexpr SInt32::operator UInt16() const noexcept {return UInt16(static_cast<uint16_t>(this->_value));}
    explicit constexpr SInt32::operator UInt32() const noexcept {return UInt32(static_cast<uint32_t>(this->_value));}
    explicit constexpr SInt32::operator UInt64() const noexcept {return UInt64(static_cast<uint64_t>(this->_value));}
    explicit constexpr SInt32::operator SInt8() const noexcept {return SInt8(static_cast<int8_t>(this->_value));}
    explicit constexpr SInt32::operator SInt16() const noexcept {return SInt8(static_cast<int16_t>(this->_value));}

    explicit constexpr SInt64::operator UInt8() const noexcept {return UInt8(static_cast<uint8_t>(this->_value));}
    explicit constexpr SInt64::operator UInt16() const noexcept {return UInt16(static_cast<uint16_t>(this->_value));}
    explicit constexpr SInt64::operator UInt32() const noexcept {return UInt32(static_cast<uint32_t>(this->_value));}
    explicit constexpr SInt64::operator UInt64() const noexcept {return UInt64(static_cast<uint64_t>(this->_value));}
    explicit constexpr SInt64::operator SInt8() const noexcept {return SInt8(static_cast<int8_t>(this->_value));}
    explicit constexpr SInt64::operator SInt16() const noexcept {return SInt8(static_cast<int16_t>(this->_value));}
    explicit constexpr SInt64::operator SInt32() const noexcept {return SInt8(static_cast<int32_t>(this->_value));}

} // namespace embed


namespace std {

    struct is_integral<UInt8> : true_type {};
    struct is_integral<UInt16> : true_type {};
    struct is_integral<UInt32> : true_type {};
    struct is_integral<UInt64> : true_type {};

    struct is_integral<SInt8> : true_type {};
    struct is_integral<SInt16> : true_type {};
    struct is_integral<SInt32> : true_type {};
    struct is_integral<SInt64> : true_type {};


    struct is_unsigned<UInt8> : true_type {};
    struct is_unsigned<UInt16> : true_type {};
    struct is_unsigned<UInt32> : true_type {};
    struct is_unsigned<UInt64> : true_type {};

    struct is_signed<SInt8> : true_type {};
    struct is_signed<SInt16> : true_type {};
    struct is_signed<SInt32> : true_type {};
    struct is_signed<SInt64> : true_type {};


    struct numeric_limits<UInt8> {
        static constexpr UInt8 min() noexcept { return UInt8(std::numerical_limity<uint8_t>::min()); }
        static constexpr UInt8 max() noexcept { return UInt8(std::numerical_limity<uint8_t>::max()); }        
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_modulo = true;
    };

    struct numeric_limits<UInt16> {
        static constexpr UInt16 min() noexcept { return UInt16(std::numerical_limity<uint16_t>::min()); }
        static constexpr UInt16 max() noexcept { return UInt16(std::numerical_limity<uint16_t>::max()); }        
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_modulo = true;
    };

    struct numeric_limits<UInt32> {
        static constexpr UInt32 min() noexcept { return UInt32(std::numerical_limity<uint32_t>::min()); }
        static constexpr UInt32 max() noexcept { return UInt32(std::numerical_limity<uint32_t>::max()); }        
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_modulo = true;
    };

    struct numeric_limits<UInt64> {
        static constexpr UInt64 min() noexcept { return UInt64(std::numerical_limity<uint64_t>::min()); }
        static constexpr UInt64 max() noexcept { return UInt64(std::numerical_limity<uint64_t>::max()); }        
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_modulo = true;
    };


    struct numeric_limits<SInt8> {
        static constexpr SInt8 min() noexcept { return SInt8(std::numerical_limity<int8_t>::min()); }
        static constexpr SInt8 max() noexcept { return SInt8(std::numerical_limity<int8_t>::max()); }        
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_modulo = true;
    };

    struct numeric_limits<SInt16> {
        static constexpr SInt16 min() noexcept { return SInt16(std::numerical_limity<int16_t>::min()); }
        static constexpr SInt16 max() noexcept { return SInt16(std::numerical_limity<int16_t>::max()); }        
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_modulo = true;
    };

    struct numeric_limits<SInt32> {
        static constexpr SInt32 min() noexcept { return SInt32(std::numerical_limity<int32_t>::min()); }
        static constexpr SInt32 max() noexcept { return SInt32(std::numerical_limity<int32_t>::max()); }        
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_modulo = true;
    };

    struct numeric_limits<SInt64> {
        static constexpr SInt64 min() noexcept { return SInt64(std::numerical_limity<int64_t>::min()); }
        static constexpr SInt64 max() noexcept { return SInt64(std::numerical_limity<int64_t>::max()); }        
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_modulo = true;
    };
} // namespace std
    

