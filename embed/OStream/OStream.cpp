#include <embed/OStream/OStream.hpp>

#include <limits>
#include <ratio>
#include <chrono>

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

    void print_num_stringified(OStream& stream, std::string_view sign_str, std::string_view num_str, const FormatIntParams& params){
        print_num_stringified(stream, sign_str, num_str, "", params);

        //const int padding_count = params._mwidth - static_cast<int>(sign_str.size()) - static_cast<int>(num_str.size());
        //const int left_padding_count = padding_count / 2;
        //const int right_padding_count = padding_count - left_padding_count;
        //const FormatStr padding = FormatStr("").fill(params._fill).mwidth(padding_count);
        //const FormatStr left_padding = FormatStr("").fill(params._fill).mwidth(left_padding_count);
        //const FormatStr right_padding = FormatStr("").fill(params._fill).mwidth(right_padding_count);
        //
        //switch(params._alignment){
        //    case AlignmentLRC::Left : {
        //        // padding after sign
        //        stream << sign_str << num_str << padding;
        //    }break;
        //    case AlignmentLRC::Center : {
        //        if(params._pad_sign){
        //            // padding between sign and number and after number
        //            stream << sign_str << left_padding << num_str << right_padding;
        //        }else{
        //            // padding before sign and after number
        //            stream << left_padding << sign_str << num_str << right_padding;
        //        }
        //    }break;
        //    default: //fallthrough
        //    case AlignmentLRC::Right : {
        //        if(params._pad_sign){
        //            // padding between sign and number
        //            stream << sign_str << padding << num_str;
        //        }else{
        //            // padding before sign
        //            stream << padding << sign_str << num_str;
        //        }
        //    }break;
        //}
    }

    void print_num_stringified(OStream& stream, std::string_view sign_str, std::string_view num_str, std::string_view suffix, const FormatIntSuffixParams& params){
        const int padding = params._mwidth - static_cast<int>(sign_str.size()) - static_cast<int>(num_str.size()) - static_cast<int>(suffix.size());
        

        int padding_before_sign = 0;
        int padding_between_sign_number = 0;
        int padding_between_number_suffix = 0;
        int padding_after_suffix = 0; 

        switch(params._alignment){
            case AlignmentLRC::Left : {
                if(params._pad_suffix){
                    // padding between number and suffix
                    padding_between_number_suffix = padding;
                }else{
                    // padding after suffix
                    padding_after_suffix = padding;
                }
            }break;
            case AlignmentLRC::Center : {
                const int left_padding = padding / 2;
                const int right_padding = padding - left_padding;

                if(!params._pad_sign && !params._pad_suffix){
                    // padding before sign and after suffix
                    padding_before_sign = left_padding;
                    padding_after_suffix = right_padding;
                }else if(params._pad_sign && !params._pad_suffix){
                    // padding between sign and number and after suffix
                    padding_between_sign_number = left_padding;
                    padding_after_suffix = right_padding;
                }else if(!params._pad_sign && params._pad_suffix){
                    // padding before sign and between number and suffix
                    padding_before_sign = left_padding;
                    padding_between_number_suffix = right_padding;
                }else /* (params._pad_sign && params._pad_suffix) */ {
                    // padding between sign and number and between number and suffix
                    padding_between_sign_number = left_padding;
                    padding_between_number_suffix = right_padding;
                }
            }break;
            case AlignmentLRC::Right : {
                if(params._pad_sign){
                    // padding between sign and number
                    padding_between_sign_number = padding;
                }else{
                    // padding before sign
                    padding_before_sign = padding;
                }
            }break;
        }

        stream.put(params._fill, padding_before_sign);
        stream.write(sign_str.data(), sign_str.size());
        stream.put(params._fill, padding_between_sign_number);
        stream.write(num_str.data(), num_str.size());
        stream.put(params._fill, padding_between_number_suffix);
        stream.write(suffix.data(), suffix.size());
        stream.put(params._fill, padding_after_suffix);
    }

    // -----------------------------------------------------------------------------------------------
    //                                       Float Formating
    // -----------------------------------------------------------------------------------------------

    char* str_add_float(char* first, char* last, const FormatFloat& value){
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
        char buffer[32];
        const std::string_view digit_str = uint_to_string(&buffer[0], &buffer[32], digits, value._use_thousands, value._thousands_char);
        for(char c : digit_str){
            *itr++ = c;
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
            if(exponent10 >= 0){
                if(value._force_exponent_sign){
                    *itr++ = '+';
                }
                std::string_view exp_str = uint_to_string(itr, last, static_cast<unsigned int>(exponent10));
                for(char c : exp_str){
                    *itr++ = c;
                }
            }else{
                *itr++ = '-';
                std::string_view exp_str = uint_to_string(itr, last, static_cast<unsigned int>(-exponent10));
                for(char c : exp_str){
                    *itr++ = c;
                }
            }
        }

        return itr;
    }


    OStream& operator<<(OStream& stream, FormatFloat value){     
        
        if(embed::is_nan(value._value)){
            return stream << FormatStr::like("nan", 3, value);
        }else{
            char buffer[64];
            char* beginItr = buffer;
            char* endItr = buffer + sizeof(buffer);
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


}// namespace embed