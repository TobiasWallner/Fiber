#include "embed/OStream.hpp"

#include <limits>

namespace embed{

    // -----------------------------------------------------------------------------------------------
    //                           Stream References and Default Streams
    // -----------------------------------------------------------------------------------------------

    OStreamRef cout;

    OStreamRef cerr;
    
    OStreamRef clog;

    // -----------------------------------------------------------------------------------------------
    //                                           OStream
    // -----------------------------------------------------------------------------------------------

    void OStream::put(char c, int count){
        for(int i = 0; i < count; ++i) this->put(c);
    }

    void OStream::write(const char* str, size_t len){
        for(size_t i = 0; i < len; ++i) this->put(*str);
    }

    void OStream::write(const char* str){
        for(; *str != 0; ++str) this->put(*str);
    }

    void OStream::newl(){
        this->put('\n');
    }

    void OStream::endl(){
        this->newl();
        this->flush();
    }

    // -----------------------------------------------------------------------------------------------
    //                                       String Formating
    // -----------------------------------------------------------------------------------------------

    OStream& operator<<(OStream& stream, const FormatStr& fstr){
        const int padding = fstr._mwidth - fstr._len;
        int left_padding = 0;
        int right_padding = 0;
        switch(fstr._alignment){
            break; case AlignmentLRC::Left: 
                right_padding = padding;
            break; case AlignmentLRC::Right: 
                left_padding = padding;
            break; case AlignmentLRC::Center: 
                right_padding = padding/2; 
                left_padding = padding - right_padding;
            break; default: 
                left_padding = 0;
                right_padding = 0;
        }
        stream.put(fstr._fill, left_padding);
        stream.write(fstr._str, fstr._len);
        stream.put(fstr._fill, right_padding);
        return stream;
    }

    // -----------------------------------------------------------------------------------------------
    //                                       Boolean Formating
    // -----------------------------------------------------------------------------------------------

    OStream& operator<<(OStream& stream, const FormatBool& value){
        if(value._to_text){
            if(value._value){
                return stream << FormatStr::like("true", 4, value);
            }else{
                return stream << FormatStr::like("false", 5, value);
            }
        }else{
            if(value._value){
                return stream << FormatStr::like("1", 1, value);
            }else{
                return stream << FormatStr::like("0", 1, value);
            }
        }
    }

    // -----------------------------------------------------------------------------------------------
    //                                       Integer Formating
    // -----------------------------------------------------------------------------------------------

    char* str_add_uint(char * first, char const * last, std::make_unsigned_t<FormatInt::value_type> value, const str_add_uint_params& params){
        char * itr = first; 

        if(value == 0){
            *itr++ = '0';
            return itr;
        }

        // convert to string ... but it is in reverse
        for(size_t i = 0; value != 0 && itr != last; ++i){
            const auto mod = value % 10;
            value = value / 10;
            if(params.use_thousands && (i != 0) && (i % 3 == 0)) *itr++ = params.thousands_char;
            *itr++ = '0' + static_cast<char>(mod);
        }

        // reverse the string
        {
            char * fwd_itr = first;
            char * bwd_itr = itr-1;
            for(; fwd_itr < bwd_itr; ++fwd_itr, --bwd_itr){
                const auto temp = *fwd_itr;
                *fwd_itr = *bwd_itr;
                *bwd_itr = temp;
            }
        }

        return itr;
    }

    char* str_add_sint(char * first, char const * last, FormatInt::value_type value, const str_add_uint_params& params){
        const bool sign = value < 0;
        const FormatInt::value_type abs_value = sign ? -value : value;
        const std::make_unsigned_t<FormatInt::value_type> uvalue = static_cast<std::make_unsigned_t<FormatInt::value_type>>(abs_value);

        if(sign){
            char * itr = first;
            *itr++ = '-';
            str_add_uint_params u_params = params;
            u_params.force_sign = false;
            return str_add_uint(itr, last, uvalue, u_params);
        }else{
            return str_add_uint(first, last, uvalue, params);
        }
    }

    OStream& operator<<(OStream& stream, const FormatInt& fvalue){
        bool sign = fvalue._value < 0;
        const FormatInt::value_type abs_value = sign ? -fvalue._value : fvalue._value;
        const std::make_unsigned_t<FormatInt::value_type> uvalue = static_cast<std::make_unsigned_t<FormatInt::value_type>>(abs_value);
        const char sign_char = (sign ? '-' : '+');
        
        char buffer[24];
        
        char const * const buffer_begin = &buffer[0];
        char const * const buffer_end = buffer_begin + sizeof(buffer);
        char * itr = buffer;

        const bool show_sign = sign || fvalue._force_sign;

        int mwidth = fvalue._mwidth;
        if(show_sign){
            if(fvalue._pad_sign){
                // add sign first to the stream and then align the number string
                stream << sign_char;
                mwidth -= 1;
            }else{
                // add sign to the number and then align the number string in the stream
                *itr++ = sign_char;
            }
        }

        str_add_uint_params params;
        params.use_thousands = fvalue._use_thousands;
        params.thousands_char = fvalue._thousands_char;

        itr = str_add_uint(itr, buffer_end, uvalue, params);

        return stream << FormatStr::like(buffer_begin, itr, fvalue).mwidth(mwidth);
    }

    // -----------------------------------------------------------------------------------------------
    //                                       Float Formating
    // -----------------------------------------------------------------------------------------------

    float frexp10(float value, int* exponent10){
        // extract exponent and mantissa
        int exponent;
        float mantissa = std::frexp(value, &exponent);

        // convert to base 10
        const float log10_2 = float(0.30102999566f); // log_10(2)
        const float log2_10 = float(3.32192809489f); // log_2(10)
        const float exponent_b10_f = exponent * log10_2;
        const int exponent_b10_i = static_cast<int>(exponent_b10_f);
        const float q = exponent_b10_f - exponent_b10_i;
        const float mantissa_b10 = mantissa * std::exp2(q * log2_10);

        *exponent10 = exponent_b10_i;
        return mantissa_b10;
    }

    char* str_add_float_ff(char* first, const char* last, unsigned long long fixpoint, unsigned long long pow_10_p_decimals, int exponent10, const str_add_float_params& params){
        char* itr = first;

        const unsigned long long digits = fixpoint / pow_10_p_decimals;
        const unsigned long long fractions = fixpoint - digits * pow_10_p_decimals;

        // convert digits to text
        {
            
            str_add_uint_params ui_params;
            ui_params.thousands_char = params.thousands_char;
            ui_params.use_thousands = params.use_thousands;
            itr = str_add_uint(itr, last, digits, ui_params);
        }

        // convert fractions to text
        {    
            if(fractions != 0 || params.force_comma || params.force_decimals){
                *itr++ = params.comma_char;
                unsigned long long temp_fractions = fractions;
                for(size_t i = 0; (i < params.decimals) && ((temp_fractions > 0) || params.force_decimals); ++i){
                    if((i != 0) && params.use_thousands && (i % 3 == 0)){
                        *itr++ = params.thousands_char;
                    } 
                    temp_fractions *= 10;
                    const unsigned long long digit = temp_fractions / pow_10_p_decimals;
                    *itr++ = '0' + static_cast<char>(digit);
                    temp_fractions = temp_fractions - digit * pow_10_p_decimals;
                }
            }
        }

        // convert exponent to string
        if((exponent10 != 0) || params.force_exponent){
            *itr++ = 'e';
            str_add_uint_params ui_params;
            ui_params.use_thousands = false;
            ui_params.force_sign = params.force_exponent_sign;
            itr = str_add_sint(itr, last, exponent10, ui_params);
        }

        return itr;
    }

    char* str_add_float(char* first, const char* last, float value, const str_add_float_params& params){
        char* itr = first;

        // get unsigned float and print sign
        float unsigned_float = std::abs(value);
        if(value < 0){
            *itr++ = '-';
        }else if(params.force_sign){
            *itr++ = '+';
        }
        
        // get exponent and mantissa in base 10: m * 10 ^ e
        int exponent10;
        float mantissa10 = frexp10(unsigned_float, &exponent10);
        
        // make sure that the mantissa (in base 10) is in the range of [1, 10)
        if(mantissa10 < 1){
            mantissa10 *= 10;
            exponent10 -= 1;
        }

        // scale the base 10 exponent and mantisse relative to the representation
        // TODO: Full representation might have issues for very large or very small numbers.
        switch(params.representation){
            case FloatRepresentation::Engineering : {
                const int em = exponent10 % 3;
                mantissa10 *= embed::pow(10, em);
                exponent10 -= em;
            }break;
            case FloatRepresentation::Full : {
                mantissa10 *= embed::pow(10, exponent10);
                exponent10 = 0;
            }break;
        }

        // round to the number of decimals
        const unsigned long long pow_10_p_decimals = embed::pow(10, params.decimals);
        const unsigned long long fixpoint = static_cast<unsigned long long>(mantissa10 * pow_10_p_decimals + 0.5f);

        return str_add_float_ff(itr, last, fixpoint, pow_10_p_decimals, exponent10, params);
    }


    OStream& operator<<(OStream& stream, FormatFloat value){     
        
        if(std::isnan(value._value)){
            return stream << FormatStr::like("nan", 3, value);
        }else if (std::isinf(value._value)) {
            if(value._value > 0){
                if(value._force_sign){
                    return stream << FormatStr::like("+inf", 4, value);
                }else{
                    return stream << FormatStr::like("inf", 3, value);
                }
            }else{
                return stream << FormatStr::like("-inf", 4, value);
            }    
        }else{
            char buffer[64];
            char const * const beginItr = buffer;
            char const * const endItr = buffer + sizeof(buffer);
            char * itr = buffer;

            const bool sign = value._value < 0;
            const char sign_char = sign ? '-' : '+';
            const bool show_sign = sign || value._force_sign;

            int mwidth = value._mwidth;
            if(show_sign){
                if(value._pad_sign){
                    // add sign first to the stream and then align the number string
                    stream << sign_char;
                    mwidth -= 1;
                }else{
                    // add sign to the number and then align the number string in the stream
                    *itr++ = sign_char;
                }
            }

            str_add_float_params fparams;
            fparams.comma_char = value._comma;
            fparams.decimals = value._decimals;
            fparams.force_comma = value._force_comma;
            fparams.force_decimals = value._force_decimals;
            fparams.force_exponent = value._force_exponent;
            fparams.force_exponent_sign = value._force_exponent_sign;
            fparams.force_sign = value._force_sign;
            fparams.thousands_char = value._thousands_char;
            fparams.use_thousands = value._use_thousands;
            fparams.representation = value._representation;
            itr = str_add_float(itr, endItr, std::abs(value._value), fparams);

            return stream << FormatStr::like(beginItr, itr, value).mwidth(mwidth);
        }
    }

    

    // -----------------------------------------------------------------------------------------------
    //                                    chrono overloads
    // -----------------------------------------------------------------------------------------------

    OStream& operator<<(OStream& stream, std::chrono::nanoseconds value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 'n';
        *itr++ = 's';
        return stream << FormatStr(first, itr);
    }

    OStream& operator<<(OStream& stream, std::chrono::microseconds value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 'u';
        *itr++ = 's';
        return stream << FormatStr(first, itr);
    }

    OStream& operator<<(OStream& stream, std::chrono::milliseconds value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 'm';
        *itr++ = 's';
        return stream << FormatStr(first, itr);
    }

    OStream& operator<<(OStream& stream, std::chrono::seconds value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 's';
        return stream << FormatStr(first, itr);
    }

    OStream& operator<<(OStream& stream, std::chrono::minutes value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 'h';
        return stream << FormatStr(first, itr);
    }

#if __cplusplus >= 202002L

    OStream& operator<<(OStream& stream, std::chrono::days value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 'd';
        return stream << FormatStr(first, itr);
    }

    OStream& operator<<(OStream& stream, std::chrono::weeks value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 'w';
        return stream << FormatStr(first, itr);
    }

    OStream& operator<<(OStream& stream, std::chrono::months value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 'M';
        return stream << FormatStr(first, itr);
    }

    OStream& operator<<(OStream& stream, std::chrono::years value){
        char buffer[24];
        const char* first = &buffer[0];
        char* itr = &buffer[0];
        const char* last = &buffer[24];
        const str_add_uint_params params;
        itr = str_add_sint(itr, last, value.count(), params);
        *itr++ = 'y';
        return stream << FormatStr(first, itr);
    }

#endif

    // -----------------------------------------------------------------------------------------------
    //                                          Tests
    // -----------------------------------------------------------------------------------------------

    void OutputStream_test_string_formating(OStream& stream){
        stream << "test string formating" << newl;
        stream << "---------------------" << newl;
        stream << "'" << FormatStr("String", 6) << "', Expected: 'String'" << newl;
        stream << "'" << FormatStr("String") << "', Expected: 'String'" << newl;
        stream << "'" << FormatStr("String").mwidth(10) << "', Expected: '    String'" << newl;
        stream << "'" << FormatStr("String").mwidth(10).left() << "', Expected: 'String    '" << newl;
        stream << "'" << FormatStr("String").mwidth(10).fill('.') << "', Expected: '....String'" << newl;
        stream << "'" << FormatStr("String").mwidth(10).left().fill('.') << "', Expected: 'String....'" << newl;
        
        FormatStr lformat = FormatStr().mwidth(64).fill('.').left();
        FormatStr rformat = FormatStr().mwidth(8).fill('.');
        stream << lformat("Chapter 1 The Beginning") << rformat("1") << endl;
        stream << lformat("Chapter 2 The Action") << rformat("22") << endl;
        stream << lformat("Chapter 3 The End") << rformat("333") << endl;
        stream << "Expected: " << newl;
        stream << "Chapter 1 The Beginning................................................1" << newl;
        stream << "Chapter 2 The Action..................................................22" << newl;
        stream << "Chapter 3 The End....................................................333" << newl;
        stream << "---" << newl;
        stream << "Alignment [" << FormatStr("Left").mwidth(10).left() << "]" << endl;
        stream << "Alignment [" << FormatStr("Right").mwidth(10).right() << "]" << endl;
        stream << "Alignment [" << FormatStr("Center").mwidth(10).center() << "]" << endl;
        stream << "Expected: " << newl;
        stream << "Alignment [Left      ]" << newl;
        stream << "Alignment [     Right]" << newl;
        stream << "Alignment [  Center  ]" << newl;
        stream << "---" << newl;
    }

    void OutputStream_test_bool_formating(OStream& stream){
        stream << "test boolean formating" << newl;
        stream << "----------------------" << newl;
        stream << "'" << true << "', Expected: 'true'" << newl;
        stream << "'" << false << "', Expected: 'false'" << newl;
        stream << "'" << FormatBool(true).num() << "', Expected: '1'" << newl;
        stream << "'" << FormatBool(false).num() << "', Expected: '0'" << newl;
        stream << "'" << FormatBool(true).text() << "', Expected: 'true'" << newl;
        stream << "'" << FormatBool(false).text() << "', Expected: 'false'" << newl;
        stream << "'" << FormatBool(true).mwidth(6) << "', Expected: '  true'" << newl;
        stream << "'" << FormatBool(false).mwidth(6) << "', Expected: ' false'" << newl;
        stream << "'" << FormatBool(true).num().mwidth(6) << "', Expected: '     1'" << newl;
        stream << "'" << FormatBool(false).num().mwidth(6) << "', Expected: '     0'" << newl;
        stream << "'" << FormatBool(true).mwidth(6).left() << "', Expected: 'true  '" << newl;
        stream << "'" << FormatBool(false).mwidth(6).left() << "', Expected: 'false '" << newl;
        stream << "'" << FormatBool(true).num().mwidth(6).left() << "', Expected: '1     '" << newl;
        stream << "'" << FormatBool(false).num().mwidth(6).left() << "', Expected: '0     '" << newl;
        stream << "'" << FormatBool(true).mwidth(6).center() << "', Expected: ' true '" << newl;
        stream << "'" << FormatBool(false).mwidth(6).center() << "', Expected: ' false'" << newl;
        stream << "'" << FormatBool(true).num().mwidth(6).center() << "', Expected: '   1  '" << newl;
        stream << "'" << FormatBool(false).num().mwidth(6).center() << "', Expected: '   0  '" << newl;
    }


    void OutputStream_test_integer_formating(OStream& stream){
        stream << "test integer formating" << newl;
        stream << "----------------------" << newl;
        stream << "'" << 42L << "', Expected: '42'" << newl;
        stream << "'" << FormatInt(42).fsign() << "', Expected: '+42'" << newl;
        stream << "'" << -42 << "', Expected: '-42'" << newl;
        stream << "'" << 42UL << "', Expected: '42'" << newl;
        stream << "'" << 0ULL << "', Expected: '0'" << newl;
        stream << "'" << FormatInt(32'156'584LL).thousands('.') << "', Expected: '32.156.584'" << newl;
        stream << "'" << FormatInt(32'156'584L).thousands() << "', Expected: '32,156,584'" << newl;
        stream << "'" << FormatInt(42U).mwidth(6) << "', Expected: '    42'" << newl;
        stream << "'" << FormatInt(42).mwidth(6).fsign() << "', Expected: '   +42'" << newl;
        stream << "'" << FormatInt(-42).mwidth(6) << "', Expected: '   -42'" << newl;
        stream << "'" << FormatInt(42).mwidth(6).fsign().pad_sign() << "', Expected: '+   42'" << newl;
        stream << "'" << FormatInt(-42).mwidth(6).pad_sign() << "', Expected: '-   42'" << newl;
        stream << "'" << FormatInt((uint8_t)42).mwidth(6).fsign().pad_sign().fill('0') << "', Expected: '+00042'" << newl;
        stream << "'" << FormatInt((int16_t)-42).mwidth(6).pad_sign().fill('0') << "', Expected: '-00042'" << newl;
        stream << "'" << FormatInt((uint32_t)42).mwidth(6).fsign().pad_sign().fill('0').left() << "', Expected: '+42000'" << newl;
        stream << "'" << FormatInt((int64_t)-42).mwidth(6).pad_sign().fill('0').left() << "', Expected: '-42000'" << newl;
        stream << "'" << FormatInt((int32_t)42).mwidth(6).left() << "', Expected: '42    '" << newl;
        stream << "'" << FormatInt((int8_t)-42).mwidth(6).left() << "', Expected: '-42   '" << newl;
        stream << "'" << FormatInt((int32_t)42).mwidth(6).center() << "', Expected: '  42  '" << newl;
        stream << "'" << FormatInt((int32_t)42).mwidth(6).center().fsign() << "', Expected: '  +42 '" << newl;
        stream << "'" << FormatInt((int8_t)-42).mwidth(6).center() << "', Expected: '  -42 '" << newl;
    }

    void OutputStream_test_float_formating(OStream& stream){
        stream << "test float formating" << newl;
        stream << "----------------------" << newl;
        stream << "'" << 3.536f << "', Expected: '3.536'" << newl;
        stream << "'" << 3.536 << "', Expected: '3.536'" << newl;
        stream << "'" << 3.536l << "', Expected: '3.536'" << newl;
        stream << "'" << 2.0 << "', Expected: '2'" << newl;
        
        stream << "'" << FormatFloat(3.536f).fexp() << "', Expected: '3.536e0'" << newl;
        stream << "'" << FormatFloat(2.0).fdeci() << "', Expected: '2.000'" << newl;
        stream << "'" << FormatFloat(2.0).fcomma() << "', Expected: '2.'" << newl;
        
        stream << "science formating:" << newl;
        stream << "'" << FormatFloat(1.23e0).sci() << "', Expected: '1.23'" << newl;
        stream << "'" << FormatFloat(1.23e1).sci() << "', Expected: '1.23e1'" << newl;
        stream << "'" << FormatFloat(1.23e2).sci() << "', Expected: '1.23e2'" << newl;
        stream << "'" << FormatFloat(1.23e3).sci() << "', Expected: '1.23e3'" << newl;
        stream << "'" << FormatFloat(23165546.231564).sci() << "', Expected: '2.317e7'" << newl;
        
        stream << "engineering formating:" << newl;
        stream << "'" << FormatFloat(1.23e0).eng() << "', Expected: '1.23'" << newl;
        stream << "'" << FormatFloat(1.23e1).eng() << "', Expected: '12.3'" << newl;
        stream << "'" << FormatFloat(1.23e2).eng() << "', Expected: '123'" << newl;
        stream << "'" << FormatFloat(1.23e3).eng() << "', Expected: '1.23e3'" << newl;
        stream << "'" << FormatFloat(23165546.231564).eng() << "', Expected: '23.166e6'" << newl;

        stream << "full formating:" << newl;
        stream << "'" << FormatFloat(1.23e0).full() << "', Expected: '1.23'" << newl;
        stream << "'" << FormatFloat(1.23e1).full() << "', Expected: '12.3'" << newl;
        stream << "'" << FormatFloat(1.23e2).full() << "', Expected: '123'" << newl;
        stream << "'" << FormatFloat(1.23e3).full() << "', Expected: '1230'" << newl;
        stream << "'" << FormatFloat(23165546.231564).full() << "', Expected: '23165546.232'" << newl;
        stream << "'" << FormatFloat(23165546.231564).full().thousands().decimals(5) << "', Expected: '23,165,546.231,56'" << newl;

        stream << "special formating:" << newl;
        float nan_val = std::nanf("");                      // NaN
        float pos_inf = std::numeric_limits<float>::infinity(); // +Infinity
        float neg_inf = -std::numeric_limits<float>::infinity(); // -Infinity
        stream << "'" << nan_val << "', Expected: 'nan'" << newl;
        stream << "'" << pos_inf << "', Expected: 'inf'" << newl;
        stream << "'" << FormatFloat(pos_inf).fsign() << "', Expected: '+inf'" << newl;
        stream << "'" << neg_inf << "', Expected: '-inf'" << newl;

    }

    void OutputStream_test(OStream& stream){
        stream << OutputStream_test_string_formating << newl;
        stream << OutputStream_test_bool_formating << newl;
        stream << OutputStream_test_integer_formating << newl;
        stream << OutputStream_test_float_formating << newl;
        
        stream.flush();
    }

}// namespace embed