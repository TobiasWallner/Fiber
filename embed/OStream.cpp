#include "embed/OStream.hpp"

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

    float frexp10(float value, int* exp10_out){
        // reinterpret float bits
        union { float f; uint32_t i; } u = { value };

        // extract base-2 exponent
        const int raw_exp2 = ((u.i >> 23) & 0xFF);
        const int exp2 = raw_exp2 - 127;

        // normalize mantissa to [0.5, 1.0)
        u.i = (u.i & 0x807FFFFF) | (127 << 23);

        // approximate log10(2) * exp2 = exp10
        int exp10 = (exp2 * 1233) >> 12;

        // precise version
        // ---------------
        //
        // lookup table calculates the earlier std::exp2((exponent_b10_f - exponent_b10_i) * log2_10)
        const float correction[] = {0.587747,  0.117549,  0.235099,  0.470198,  0.940395,  0.188079,  0.376158,  0.752316,  0.150463,  0.300927,  0.601853,  0.120371,  0.240741,  0.481482,  0.962965,  0.192593,  0.385186,  0.770372,  0.154074,  0.308149,  0.616298,  0.123260,  0.246519,  0.493038,  0.986076,  0.197215,  0.394430,  0.788861,  0.157772,  0.315544,  0.631089,  0.126218,  0.252435,  0.504871,  0.100974,  0.201948,  0.403897,  0.807794,  0.161559,  0.323117,  0.646235,  0.129247,  0.258494,  0.516988,  0.103398,  0.206795,  0.413590,  0.827181,  0.165436,  0.330872,  0.661744,  0.132349,  0.264698,  0.529396,  0.105879,  0.211758,  0.423516,  0.847033,  0.169407,  0.338813,  0.677626,  0.135525,  0.271051,  0.542101,  0.108420,  0.216840,  0.433681,  0.867362,  0.173472, 0.346945,  0.693889,  0.138778,  0.277556,  0.555112,  0.111022,  0.222045,  0.444089,  0.888178,  0.177636,  0.355271,  0.710543,  0.142109,  0.284217,  0.568434,  0.113687,  0.227374,  0.454747,  0.909495,  0.181899,  0.363798,  0.727596,  0.145519,  0.291038,  0.582077,  0.116415, 0.232831,  0.465661,  0.931323,  0.186265,  0.372529,  0.745058,  0.149012,  0.298023,  0.596046,  0.119209,  0.238419,  0.476837,  0.953674,  0.190735,  0.381470,  0.762939,  0.152588,  0.305176,  0.610352,  0.122070,  0.244141,  0.488281,  0.976562,  0.195312,  0.390625,  0.781250,  0.156250,  0.312500,  0.625000,  0.125000,  0.250000,  0.500000,  1.000000,  2.000000,  4.000000,  8.000000,  1.600000,  3.200000,  6.400000,  1.280000,  2.560000,  5.120000,  1.024000,  2.048000,  4.096000,  8.192000,  1.638400,  3.276800,  6.553600,  1.310720,  2.621440,  5.242880,  1.048576,  2.097152,  4.194304,  8.388608,  1.677722,  3.355443,  6.710886,  1.342177,  2.684355,  5.368709,  1.073742,  2.147484,  4.294967,  8.589935,  1.717987,  3.435974,  6.871948,  1.374390,  2.748779,  5.497558,  1.099512,  2.199023,  4.398047,  8.796093,  1.759219,  3.518437,  7.036874,  1.407375,  2.814750,  5.629500,  1.125900,  2.251800,  4.503600,  9.007199,  1.801440,  3.602880,  7.205759,  1.441152,  2.882304,  5.764608,  1.152922,  2.305843,  4.611686, 9.223372,  1.844674,  3.689349,  7.378698,  1.475740,  2.951479,  5.902958,  1.180592,  2.361183,  4.722366,  9.444733,  1.888947,  3.777893,  7.555786,  1.511157,  3.022315,  6.044629,  1.208926,  2.417852,  4.835703,  9.671407,  1.934281,  3.868563,  7.737125,  1.547425,  3.094850,  6.189700,  1.237940,  2.475880,  4.951760,  9.903520,  1.980704,  3.961408,  7.922816,  1.584563,  3.169127,  6.338253,  1.267651,  2.535301,  5.070602,  1.014120,  2.028241,  4.056482,  8.112964,  1.622593,  3.245186,  6.490371,  1.298074,  2.596148,  5.192297,  1.038459,  2.076919,  4.153837,  8.307675,  1.661535,  3.323070,  6.646140,  1.329228,  2.658456,  5.316912,  1.063382,  2.126765,  4.253530,  8.507059,  1.701412};
        float mant10 = u.f * correction[raw_exp2];
        
        // RAM friendly but inprecise version
        // ----------------------------------
        //
        // replace original float table with uint16_t encoded floats to save memory
        //constexpr uint16_t negative_correction[] = {38518, 7703, 15407, 30814, 61629, 12325, 24651, 49303, 9860, 19721, 39443, 7888, 15777, 31554, 63108, 12621, 25243, 50487, 10097, 20194, 40389, 8077, 16155, 32311, 64623, 12924, 25849, 51698, 10339, 20679, 41359, 8271, 16543, 33087, 6617, 13234, 26469, 52939, 10587, 21175, 42351, 8470, 16940, 33881, 6776, 13552, 27105, 54210, 10842, 21684, 43368, 8673, 17347, 34694, 6938, 13877, 27755, 55511, 11102, 22204, 44408, 8881, 17763, 35527, 7105, 14210, 28421, 56843, 11368, 22737, 45474, 9094, 18189, 36379, 7275, 14551, 29103, 58207, 11641, 23283, 46566, 9313, 18626, 37252, 7450, 14901, 29802, 59604, 11920, 23841, 47683, 9536, 19073, 38146, 7629, 15258, 30517, 61035, 12207, 24414, 48828, 9765, 19531, 39062, 7812, 15624, 31249, 62499, 12499, 24999, 49999, 9999, 20000, 40000, 8000, 16000, 31999, 63999, 12799, 25599, 51200, 10240, 20480, 40960, 8192, 16384};
        //constexpr uint16_t positive_correction[] = {6553, 13107, 26214, 52428, 10485, 20971, 41943, 8388, 16777, 33554, 6710, 13421, 26843, 53687, 10737, 21474, 42949, 8589, 17179, 34359, 6871, 13743, 27487, 54975, 10995, 21990, 43980, 8796, 17592, 35184, 7036, 14073, 28147, 56294, 11258, 22517, 45035, 9007, 18014, 36028, 7205, 14411, 28823, 57646, 11529, 23058, 46116, 9223, 18446, 36893, 7378, 14757, 29514, 59029, 11805, 23611, 47223, 9444, 18889, 37778, 7555, 15111, 30223, 60446, 12089, 24178, 48357, 9671, 19342, 38685, 7737, 15474, 30948, 61897, 12379, 24758, 49517, 9903, 19807, 39614, 7922, 15845, 31691, 63382, 12676, 25353, 50706, 10141, 20282, 40564, 8112, 16225, 32451, 64903, 12980, 25961, 51922, 10384, 20769, 41538, 8307, 16615, 33230, 6646, 13292, 26584, 53169, 10633, 21267, 42535, 8507, 17014, 34028, 6805, 13611, 27222, 54445, 10889, 21778, 43556, 8711, 17422, 34844, 6968, 13937, 27875, 55751, 11150};
        //constexpr float negative_uint16_to_float = 1.52587890625e-05f;
        //constexpr float positive_uint16_to_float = 0.000152587890625f;
        //float mant10;
        //
        //if(exp2 < 0){
        //    mant10 = u.f * negative_correction[raw_exp2] * negative_uint16_to_float;
        //}else{
        //    mant10 = u.f * positive_correction[exp2] * positive_uint16_to_float;
        //}

        if(mant10 >= 10.f){
            mant10 /= 10.f;
            exp10 += 1;
        }else if(mant10 < 1.f){
            mant10 *= 10.f;
            exp10 -= 1;
        }

        *exp10_out = exp10;
        return mant10;

        /*
        
        correction factors have been calculated as follows:
        import numpy as np
        import matplotlib.pyplot as plt

        exp2 = np.arange(-127, 128)
        exp10f = exp2 * np.log10(2)
        exp10i = exp10f.astype(np.int32)
        error = exp10f - exp10i
        correction = np.exp2(error * np.log2(10))

        exp2 = np.arange(-127, -1)
        exp10f = exp2 * np.log10(2)
        exp10i = exp10f.astype(np.int32)
        error = exp10f - exp10i
        negative_correction = np.exp2(error * np.log2(10))

        negative_correction_i16 = (negative_correction * 65536).astype(np.uint32)
        print("negative correction")
        for elem in negative_correction_i16:
            print(f"{elem}, ", end='')
        print("")
        print("")
        print(f"scaling factor: {1/65536.0}")
        print("")
        print("")

        exp2 = np.arange(0, 128)
        exp10f = exp2 * np.log10(2)
        exp10i = exp10f.astype(np.int32)
        error = exp10f - exp10i
        positive_correction = np.exp2(error * np.log2(10))

        positive_correction_i16 = (positive_correction * 6553.6).astype(np.uint32)
        print("positive correction")
        for elem in positive_correction_i16:
            print(f"{elem}, ", end='')
            
        print("")
        print("")
        print(f"scaling factor: {1/6553.6}")
        print("")
        print("")
        
        */
    }

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