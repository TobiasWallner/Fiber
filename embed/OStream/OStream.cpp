#include <embed/OStream/OStream.hpp>

#include <limits>

namespace embed{

// switch between compiletime and runtime member access
#ifdef EMBED_FMT_MINIMAL
    #define EMBED_FMT_MEMBER(type, var, member) type::member
#else
    #define EMBED_FMT_MEMBER(type, var, member) var.member
#endif

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
        if(EMBED_FMT_MEMBER(FormatBool, value, _to_text)){
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
            if(EMBED_FMT_MEMBER(str_add_uint_params, params, use_thousands) && (i != 0) && (i % 3 == 0)) *itr++ = EMBED_FMT_MEMBER(str_add_uint_params, params, thousands_char);
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

        const bool show_sign = sign || EMBED_FMT_MEMBER(FormatInt, fvalue, _force_sign);

        int mwidth = fvalue._mwidth;
        if(show_sign){
            if(EMBED_FMT_MEMBER(FormatInt, fvalue, _pad_sign)){
                // add sign first to the stream and then align the number string
                stream << sign_char;
                mwidth -= 1;
            }else{
                // add sign to the number and then align the number string in the stream
                *itr++ = sign_char;
            }
        }

        str_add_uint_params params;
        #ifndef EMBED_FMT_MINIMAL
            params.use_thousands = fvalue._use_thousands;
            params.thousands_char = fvalue._thousands_char;
        #endif
        itr = str_add_uint(itr, buffer_end, uvalue, params);

        return stream << FormatStr::like(buffer_begin, itr, fvalue).mwidth(mwidth);
    }

    // -----------------------------------------------------------------------------------------------
    //                                       Float Formating
    // -----------------------------------------------------------------------------------------------

    char* str_add_float(char* first, const char* last, const FormatFloat& value){
        char* itr = first;

        // get unsigned float and print sign
        float unsigned_float = value._value;
        
        // get exponent and mantissa in base 10: m * 10 ^ e
        int exponent10;
        float mantissa10 = frexp10(unsigned_float, &exponent10);

        // scale the base 10 exponent and mantisse relative to the representation
        // TODO: Full representation might have issues for very large or very small numbers.
        switch(EMBED_FMT_MEMBER(FormatFloat, value, _representation)){
            case FloatRepresentation::Engineering : {
                const int em = exponent10 % 3;
                mantissa10 *= embed::pow(10, em);
                exponent10 -= em;
            }break;
            case FloatRepresentation::Full : {
                mantissa10 *= embed::pow(10, exponent10);
                exponent10 = 0;
            }break;
            default: break;
        }

        // round to the number of decimals
        const unsigned long long pow_10_p_decimals = embed::pow(10, EMBED_FMT_MEMBER(FormatFloat, value, _decimals));
        const unsigned long long fixpoint = static_cast<unsigned long long>(mantissa10 * pow_10_p_decimals + 0.5f);

        const unsigned long long digits = fixpoint / pow_10_p_decimals;
        const unsigned long long fractions = fixpoint - digits * pow_10_p_decimals;

        // convert digits to text
        {
            
            str_add_uint_params ui_params;
            #ifndef EMBED_FMT_MINIMAL
                ui_params.thousands_char = value._thousands_char;
                ui_params.use_thousands = value._use_thousands;
            #endif
            itr = str_add_uint(itr, last, digits, ui_params);
        }

        // convert fractions to text
        {    
            if(fractions != 0 || EMBED_FMT_MEMBER(FormatFloat, value, _force_comma) || EMBED_FMT_MEMBER(FormatFloat, value, _force_decimals)){
                *itr++ = EMBED_FMT_MEMBER(FormatFloat, value, _comma);
                unsigned long long temp_fractions = fractions;
                for(size_t i = 0; (i < EMBED_FMT_MEMBER(FormatFloat, value, _decimals)) && ((temp_fractions > 0) || EMBED_FMT_MEMBER(FormatFloat, value, _force_decimals)); ++i){
                    if((i != 0) && EMBED_FMT_MEMBER(FormatFloat, value, _use_thousands) && (i % 3 == 0)){
                        *itr++ = EMBED_FMT_MEMBER(FormatFloat, value, _thousands_char);
                    } 
                    temp_fractions *= 10;
                    const unsigned long long digit = temp_fractions / pow_10_p_decimals;
                    *itr++ = '0' + static_cast<char>(digit);
                    temp_fractions = temp_fractions - digit * pow_10_p_decimals;
                }
            }
        }

        // convert exponent to string
        if((exponent10 != 0) || EMBED_FMT_MEMBER(FormatFloat, value, _force_exponent)){
            *itr++ = 'e';
            str_add_uint_params ui_params;
            #ifndef EMBED_FMT_MINIMAL
                ui_params.use_thousands = false;
                ui_params.force_sign = EMBED_FMT_MEMBER(FormatFloat, value, _force_exponent_sign);
            #endif
            itr = str_add_sint(itr, last, exponent10, ui_params);
        }

        return itr;
    }


    OStream& operator<<(OStream& stream, FormatFloat value){     
        
        if(embed::is_nan(value._value)){
            return stream << FormatStr::like("nan", 3, value);
        }else{
            char buffer[64];
            char const * const beginItr = buffer;
            char const * const endItr = buffer + sizeof(buffer);
            char * itr = buffer;


            const bool sign = value._value < 0;
            value._value = sign ? -value._value : value._value;
            const char sign_char = sign ? '-' : '+';
            const bool show_sign = sign || EMBED_FMT_MEMBER(FormatFloat, value, _force_sign);

            if(show_sign){
                if(EMBED_FMT_MEMBER(FormatFloat, value, _pad_sign)){
                    // add sign first to the stream and then align the number string
                    stream << sign_char;
                    value._mwidth -= 1;
                }else{
                    // add sign to the number and then align the number string in the stream
                    *itr++ = sign_char;
                }
            }

            if (embed::is_inf(value._value)){
                *itr++ = 'i';
                *itr++ = 'n';
                *itr++ = 'f';
            }else{
                itr = str_add_float(itr, endItr, value);
            }
            return stream << FormatStr::like(beginItr, itr, value);
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

    

}// namespace embed