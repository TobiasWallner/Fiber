#pragma once

//std
#include <cstdint>
#include <type_traits>
#include <cmath>
#include <chrono>
#include <string_view>

//embed
#include <embed/Core/concepts.hpp>
#include <embed/Core/type_traits.hpp>
#include <embed/Core/chrono.hpp>
#include <embed/math/math.hpp>
#include <embed/Exceptions/Exceptions.hpp>

namespace embed{

    #ifdef EMBED_FMT_MINIMAL
        #if defined(EMBED_FMT_DECIMALS_1)
            #define EMBED_FMT_DECIMALS 1
        #elif defined(EMBED_FMT_DECIMALS_2)
            #define EMBED_FMT_DECIMALS 2
        #elif defined(EMBED_FMT_DECIMALS_3)
            #define EMBED_FMT_DECIMALS 3
        #elif defined(EMBED_FMT_DECIMALS_4)
            #define EMBED_FMT_DECIMALS 4
        #elif defined(EMBED_FMT_DECIMALS_5)
            #define EMBED_FMT_DECIMALS 5
        #elif defined(EMBED_FMT_DECIMALS_6)
            #define EMBED_FMT_DECIMALS 6
        #endif

        #if defined(EMBED_FMT_FLOAT_REP_SCI)
            #define EMBED_FMT_FLOAT_REP FloatRepresentation::Scientific
        #elif defined(EMBED_FMT_FLOAT_REP_ENG)
            #define EMBED_FMT_FLOAT_REP FloatRepresentation::Engineering
        #else // defined(EMBED_FMT_FLOAT_REP_FULL)
            #define EMBED_FMT_FLOAT_REP FloatRepresentation::Full
        #endif

        #ifdef EMBED_FMT_DOT_AS_COMMA
            #define EMBED_FMT_COMMA_CHAR '.'
            #define EMBED_FMT_THOUSANDS_CHAR ','
        #else
            #define EMBED_FMT_COMMA_CHAR ','
            #define EMBED_FMT_THOUSANDS_CHAR '.'
        #endif

        #ifdef EMBED_FMT_BOOL_TO_TEXT
            #define EMBED_FMT_BOOL_TO_TEXT_VALUE true
        #else 
            #define EMBED_FMT_BOOL_TO_TEXT_VALUE false
        #endif

        #ifdef EMBED_FMT_THOUSANDS
            #define EMBED_FMT_THOUSANDS_VALUE true
        #else 
            #define EMBED_FMT_THOUSANDS_VALUE false
        #endif

        #ifdef EMBED_FMT_PAD_SIGN
            #define EMBED_FMT_PAD_SIGN_VALUE true
        #else 
            #define EMBED_FMT_PAD_SIGN_VALUE false
        #endif

        #ifdef EMBED_FMT_FORCE_COMMA
            #define EMBED_FMT_FORCE_COMMA_VALUE true
        #else 
            #define EMBED_FMT_FORCE_COMMA_VALUE false
        #endif

        #ifdef EMBED_FMT_FORCE_DECIMALS
            #define EMBED_FMT_FORCE_DECIMALS_VALUE true
        #else
            #define EMBED_FMT_FORCE_DECIMALS_VALUE false
        #endif

        #ifdef EMBED_FMT_FORCE_SIGN
            #define EMBED_FMT_FORCE_SIGN_VALUE true
        #else
            #define EMBED_FMT_FORCE_SIGN_VALUE false
        #endif

        #ifdef EMBED_FMT_FORCE_EXPONENT
            #define EMBED_FMT_FORCE_EXPONENT_VALUE true
        #else
            #define EMBED_FMT_FORCE_EXPONENT_VALUE false
        #endif

        #ifdef EMBED_FMT_FORCE_EXPONENT_SIGN
            #define EMBED_FMT_FORCE_EXPONENT_SIGN_VALUE true
        #else
            #define EMBED_FMT_FORCE_EXPONENT_SIGN_VALUE false
        #endif
    #endif

    /**
     * \brief Abstract class for an output character stream that offers string and number formating.
     * 
     * Do you want to implement your own output stream? For example with a custom USART driver?
     * But the standard output stream `std::ostream` or {fmt} fmt::format_to() use too much FLASH memory?
     * This streaming interface serves as a minimal implementation for streams with a small footprint.
     * Derive from this class and use its number and string formatings that let you do stuff like:
     * 
     * ```C++
     * stream << "this is an int: " << 42 << embed::newl;
     * stream << "this is a float: " << 42.32 << embed::newl;
     * stream << "this is an bool: " << true << embed::endl;
     * ```
     * 
     * Virtual methods that the user has to overload:
     * ```C++
     * virtual void put(char c);
     * virtual void flush();
     * ```
     * 
     * The other methods will use them but it is best to also overload:
     * ```C++
     * virtual void write(const char* str, size_t len)
     * ```
     * as well for best performance.
     */
    class OStream{
    public:

        virtual ~OStream(){}
        
        /**
         * \brief Overload this method to write a character to the stream
         * \details Has to be overloaded by the user!
         * \param c A character that should be written to the stream
        */
        virtual void put(char c) = 0;

        /**
         * \brief Writes a character to the stream `count` many times
         * \details Will only place characters if `count` is positive
         * \param c A character that should be written to the stream
         * \param count the number of times that character should be written to the stream
        */
       virtual void put(char c, int count);

        /**
         * \brief Overload this method to force flush the buffer
         * \details Has to be overloaded by the user!
         */
        virtual void flush() = 0;

        /**
         * \brief Writes a string to the stream.
         * \details If not implemented by the user, will use the `put()` method to write to the stream
         * \param str a pointer to the start of the string
         * \param len the length of the string, aka. the number of characters that should be read from the string
         */
        virtual void write(const char* str, size_t len);
        
        /**
         * \brief Writes a string to the stream.
         * \details If not implemented by the user, will use the `put()` method to write to the stream
         * \param str a pointer to the start of a zero-terminated string
         */
        virtual void write(const char* str);

        /**
         * \brief Prints a new line character. Compared to endl(), this function does not call flush()
         */
        virtual void newl();

        /**
         * \brief Prints a new line character followed by a call flush()
         */
        virtual void endl();
    };

    /**
     * \brief Writes a new line character to the stream
     * 
     * Calls OStream::newl().
     * Uses the new line character set by the stream with OStream::new_line_char().
     */
    inline void newl(OStream& stream){stream.newl();}

    /**
     * \brief Writes a new line character to the stream
     * 
     * Calls OStream::flush().
     */
    inline void flush(OStream& stream){stream.flush();}

    /**
     * \brief Writes a new line character to the stream followed by a call to OStream::flush()
     * 
     * Calls OStream::endl().
     * Uses the new line character set by the stream with OStream::new_line_char().
     */
    inline void endl(OStream& stream){stream.endl();}

    /**
     * \brief Calls a function with the stream as the argument.
     * 
     * Example:
     * ```
     * void function(OStream&){
     *      // do stuff
     * }
     * 
     * int main(){
     *      stream << function;
     *      // is equivalent to
     *      function(stream)
     *      
     *      // Intended usages:
     *      using namespace embed;
     *      stream << "This is followed by a new line" << newl();
     *      stream << "This line is flushed" << flush();
     *      stream << "This line is followed by a newline and flushed" << endl();
     * }
     * 
     * ```
     */
    inline OStream& operator<<(OStream& stream, void(*function)(OStream&)){
        function(stream);
        return stream;
    }

    /**
     * \brief Writes a char to the stream
     */
    inline OStream& operator<<(OStream& stream, char c){
        stream.put(c);
        return stream;
    }

    /**
     * \brief writes a terminated c-style string to the stream
     */
    inline OStream& operator<<(OStream& stream, const char* str){
        stream.write(str);
        return stream;
    }

    template<CStringView StringView>
    inline OStream& operator<<(OStream& stream, const StringView& str){
        stream.write(str.data(), str.size());
        return stream;
    }
    
    enum class AlignmentLRC{Left, Right, Center};

    struct FormatStrParams{
        AlignmentLRC _alignment = AlignmentLRC::Right;
        int _mwidth = 0;
        char _fill = ' ';
    };

    /**
     * \brief Formats a string and allows to pass an additional size parameter
     */
    struct FormatStr : public FormatStrParams{
        const char* _str = "";
        size_t _len = 0;

        /// @brief Default format constructor
        constexpr FormatStr() = default;

        /// @brief Construcs a string formater
        /// @param str Pointer to the start of the string
        /// @param len The length of the string, aka. the number of character that should be printed
        constexpr FormatStr(const char* str, size_t len) : FormatStrParams(), _str(str), _len(len){}
        
        /**
         * \brief Constructs a string from a terminated c-style string. I urge you to use FormatStr(const char* str, size_t len) instead.
         */
        constexpr FormatStr(const char* str) : FormatStr(str, string_length(str)){}

        /// @brief Constructs a string from a range given by the closed-open iterators [first, last)
        /// @param first The start of the string that should be printed. Points to the first character.
        /// @param last The end of the string that should be printed. Points past the last character.
        constexpr FormatStr(const char* first, const char* last) : FormatStr(first, last-first){}

        template<CStringView StringView>
        constexpr FormatStr(const StringView& str) : FormatStr(str.data(), str.size()){}

    private:
        // private constructors
        constexpr FormatStr(const char* str, size_t len, const FormatStrParams& params) : FormatStrParams(params), _str(str), _len(len){}
        constexpr FormatStr(const char* str, const FormatStrParams& params) : FormatStr(str, string_length(str), params){}
        constexpr FormatStr(const char* first, const char* last, const FormatStrParams& params) : FormatStr(first, last-first, params){}
        template<CStringView StringView>
        constexpr FormatStr(const StringView& str, const FormatStrParams& params) : FormatStr(str.data(), str.size(), params){}
    public:

        constexpr static FormatStr like(const char* str, size_t len, const FormatStrParams& params){return FormatStr(str, len, params);}
        constexpr static FormatStr like(const char* str, const FormatStrParams& params){return FormatStr(str, params);}
        constexpr static FormatStr like(const char* first, const char* last, const FormatStrParams& params){return FormatStr(first, last, params);}
        template<CStringView StringView>
        constexpr static FormatStr like(const StringView& str, const FormatStrParams& params){return FormatStr(str, params);}

        /**
         * \brief Assigns a string to the object. Allows to re-use formats.
         * 
         * This function allows to create custom formaters and reuse them when printing - reusing code bloat and clutter.
         * 
         * Example:
         * ```C++
         * FormatStr lformat = FormatStr().mwidth(64).fill('.').left();
         * FormatStr rformat = FormatStr().mwidth(8).fill('.');
         * 
         * stream << lformat("Chapter 1 The Beginning") << rformat("1") << endl;
         * stream << lformat("Chapter 2 The Action") << rformat("22") << endl;
         * stream << lformat("Chapter 3 The End") << rformat("333") << endl;
         * ```
         * Output:
         * ```
         * Chapter 1 The Beginning................................................1
         * Chapter 2 The Action..................................................22
         * Chapter 3 The End....................................................333
         * ```
         * 
         * \param str A terminated c-style string
         * \returns self
         */
        constexpr FormatStr& operator()(const char* str){
            this->_str = str; 
            this->_len = string_length(str);
            return *this;
        }

        /**
         * \brief Assigns a string to the object. Allows to re-use formats.
         * \details Like: FormatStr& operator()(const char* str)
         * 
         * \param str A terminated c-style string
         * \param len the length of the string
         * \returns self
         */
        constexpr FormatStr& operator()(const char* str, size_t len){
            this->_str = str; 
            this->_len = len;
            return *this;
        }

        /**
         * \brief Assigns a string to the object. Allows to re-use formats.
         * \details Like: FormatStr& operator()(const char* str)
         * \param first The start of the string that should be printed. Points to the first character.
         * \param last The end of the string that should be printed. Points past the last character.
         * \returns self
         */
        constexpr FormatStr& operator()(const char* first, const char* last){
            this->_str = first; 
            this->_len = last - first;
            return *this;
        }

        /**
         * \brief Assigns a string to the object. Allows to re-use formats.
         * \details Like: FormatStr& operator()(const char* str)
         * \param str a string view like object. Has to offer the following two methods:
         *      - data(): that returns a pointer to the start of the stream
         *      - size(): that returns the size/length of the stream
         * \returns self
         */
        template<CStringView StringView>
        constexpr FormatStr& operator()(const StringView& str){
            this->_str = str.data(); 
            this->_len = str.size();
            return *this;
        }
        
        /**
         * \brief Sets the minimum number of character that will be put into the stream. 
         * 
         * Padded characters are defined by FormatStr::fill(char c). 
         * The default fill character is a space ' '.
         * 
         * Example:
         * ```C++
         * stream << "[" << FormatStr("OK").mwidth(6) << "]" << endl; // Outputs: [    OK]
         * ```
         * 
         * \param mw An integer that sets the new minimal width
         * \returns self
         */
        constexpr FormatStr& mwidth(int mw){this->_mwidth = mw; return *this;}

        /**
         * \brief Formats the string to the left area set by mwidth().
         * 
         * Example:
         * ```C++
         * stream << "[" << FormatStr("OK").mwidth(6).left() << "]" << endl; // Outputs: [OK    ]
         * ```
         * 
         * \returns self
         */
        constexpr FormatStr& left(){this->_alignment = AlignmentLRC::Left; return *this;}

        /**
         * \brief Formats the string to the left area set by mwidth().
         * 
         * Example:
         * ```C++
         * stream << "[" << FormatStr("OK").mwidth(6).right() << "]" << endl; // Outputs: [    OK]
         * ```
         * 
         * \returns self
         */
        constexpr FormatStr& right(){this->_alignment = AlignmentLRC::Right; return *this;}

        /**
         * \brief Formats the string to the left area set by mwidth().
         * 
         * Example:
         * ```C++
         * stream << "[" << FormatStr("OK").mwidth(6).center() << "]" << endl; // Outputs: [  OK  ]
         * ```
         * 
         * \returns self
         */
        constexpr FormatStr& center(){this->_alignment = AlignmentLRC::Center; return *this;}

        /**
         * \brief Sets the fill characters used by padding that is applied when mwidth() is used.
         * \param c The new fill character
         * \returns self
         */
        constexpr FormatStr& fill(char c){this->_fill = c; return *this;}
    };

    /**
     * \brief Writes a `FormatStr` to the stream that allows to pass the string length in addition 
     * to the `const char*` type string
     */
    OStream& operator<<(OStream& stream, const FormatStr& fstr);

    struct FormatBoolParam : public FormatStrParams{
        #ifndef EMBED_FMT_MINIMAL
            bool _to_text = true;
        #else
            static constexpr bool _to_text = EMBED_FMT_BOOL_TO_TEXT_VALUE;
        #endif

        constexpr FormatBoolParam()=default;
        explicit constexpr FormatBoolParam(const FormatBoolParam&)=default;
        constexpr FormatBoolParam& operator=(const FormatBoolParam&)=default;

        explicit constexpr FormatBoolParam(const FormatStrParams& params) : FormatStrParams(params){}
        constexpr FormatBoolParam& operator=(const FormatStrParams& params){
            this->FormatStrParams::operator=(params);
            return *this;
        }
    };

    /**
     * \brief formats a bool for use with OStream
     */
    struct FormatBool : FormatBoolParam{
        bool _value;
        
        constexpr FormatBool(bool value) : FormatBoolParam(), _value(value){}

    private:
        constexpr FormatBool(bool value, const FormatStrParams& params) : FormatBoolParam(params), _value(value){}
        constexpr FormatBool(bool value, const FormatBoolParam& params) : FormatBoolParam(params), _value(value){}
    public:

        constexpr static FormatBool like(bool value, const FormatStrParams& params){return FormatBool(value, params);}
        constexpr static FormatBool like(bool value, const FormatBoolParam& params){return FormatBool(value, params);}

        /**
         * \brief enables formating to text (default)
         * \details prints the bool as text: "true"/"false" (default if not calling the function)
         */
        constexpr FormatBool& text(){
            #ifndef EMBED_FMT_MINIMAL
                this->_to_text = true; 
            #endif
            return *this;
        }

        /**
         * \brief disables formating to a numer
         * \details prints the bool as numbers: '1', '0'
         */
        constexpr FormatBool& num(){
            #ifndef EMBED_FMT_MINIMAL
                this->_to_text = false; 
            #endif
            return *this;
        }

        /**
         * \brief Aligns the number to the left
         * 
         * Example:
         * ```C++
         * stream << "'" << FormatBool(true).mwidth(6).left() << "', Expected: 'true  '" << newl;
         * stream << "'" << FormatBool(false).mwidth(6).left() << "', Expected: 'false '" << newl;
         * ```
         */
        constexpr FormatBool& left(){this->_alignment = AlignmentLRC::Left; return *this;}
        constexpr FormatBool& right(){this->_alignment = AlignmentLRC::Right; return *this;}
        constexpr FormatBool& center(){this->_alignment = AlignmentLRC::Center; return *this;}

        /**
         * \brief Set a minimum width of characters that will be printed to the stream
         * 
         * Example:
         * ```C++
         * stream << '"' << FormatBool(true) << '"' << endl;                    // Outputs: "true"
         * stream << '"' << FormatBool(true).mwidth(6) << '"' << endl;          // Outputs: "  true"
         * stream << '"' << FormatBool(true).left().mwidth(6) << '"' << endl;   // Outputs: "true  "
         * ```
         */
        constexpr FormatBool& mwidth(int mw){this->_mwidth = mw; return *this;}

        /**
         * \brief Sets the fill character used for padding. Default is a space ' '.
         */
        constexpr FormatBool& fill(char c){this->_fill = c; return *this;}
    };

    /**
     * \brief Writes a bool to the output stream and applies custom formating
     */
    OStream& operator<<(OStream& stream, const FormatBool& value);

    /**
     * \brief Default bool formating, uses the settings of the stream
     */
    inline OStream& operator<<(OStream& stream, bool value){
        return stream << FormatBool(value);
    }

// -----------------------------------------------------------------------------------------------
//                                    integer formating
// -----------------------------------------------------------------------------------------------

    struct _FormatUIntParams{
        #ifndef EMBED_FMT_MINIMAL
            char _thousands_char = ',';
            bool _use_thousands = false;
        #else
            static constexpr char _thousands_char = EMBED_FMT_THOUSANDS_CHAR;
            static constexpr bool _use_thousands = EMBED_FMT_THOUSANDS_VALUE;
        #endif
    };

    struct FormatIntParams : public FormatStrParams, public _FormatUIntParams{
        #ifndef EMBED_FMT_MINIMAL
            bool _force_sign = false;
            bool _pad_sign = false;
        #else
            static constexpr bool _force_sign = EMBED_FMT_FORCE_SIGN_VALUE;
            static constexpr bool _pad_sign = EMBED_FMT_PAD_SIGN_VALUE;
        #endif

        constexpr FormatIntParams()=default;
        constexpr FormatIntParams(const FormatIntParams&)=default;
        constexpr FormatIntParams& operator=(const FormatIntParams&)=default;

        constexpr FormatIntParams(const FormatStrParams& params) : FormatStrParams(params){}
        constexpr FormatIntParams& operator=(const FormatStrParams& params){
            this->FormatStrParams::operator=(params);
            return *this;
        }
    };

    /**
     * \brief Formats an unsigned integer (of any size) for use with OStream
     */
    template<std::integral Int>
    struct FormatInt : public FormatIntParams{
    public:
        using value_type = typename embed::make_fast<Int>::type;
        value_type _value = 0;

        constexpr FormatInt() = default;
        constexpr FormatInt(const FormatInt&) = default;
        constexpr FormatInt& operator=(const FormatInt&) = default;

        constexpr FormatInt(Int value) : _value(static_cast<value_type>(value)){}
    private:
        // private constructors
        constexpr FormatInt(value_type value, const FormatIntParams& params) : FormatIntParams(params), _value(value){}

        template <std::integral Int1>
        constexpr FormatInt(Int1 value, const FormatStrParams& params) : FormatIntParams(params), _value(static_cast<value_type>(value)){}
    public:

        static constexpr FormatInt like(value_type value, const FormatIntParams& params){return FormatInt(value, params);}

        template <std::integral Int1>
        static constexpr FormatInt like(Int1 value, const FormatStrParams& params){return FormatInt(value, params);}

        /**
         * \brief enables thousand characters
         * 
         * Example:
         * ```C++
         * stream << FormatInt(4203).use_thousands(); //Outputs 4,203
         * stream << FormatInt(4203).use_thousands(true); //Outputs 4,203
         * stream << FormatInt(4203).use_thousands(false); //Outputs 4203
         * ```
         */
        constexpr FormatInt& use_thousands([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_use_thousands = b; 
            #endif
            return *this;
        }

        /**
         * \brief enables thousand characters
         * 
         * Example:
         * ```C++
         * stream << FormatInt(4203).thousands('.'); //Outputs 4.203
         * stream << FormatInt(4203).thousands(','); //Outputs 4,203
         * ```
         */
        constexpr FormatInt& thousands([[maybe_unused]]char c=','){
            #ifndef EMBED_FMT_MINIMAL
                this->_thousands_char = c; 
                this->_use_thousands = true; 
            #endif
            return *this;
        }

        /// @brief force the printing of the sign, even if it is positive. 
        constexpr FormatInt& fsign([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_force_sign = b; 
            #endif
            return *this;
        }
        constexpr FormatInt& mwidth(int mw){this->_mwidth = mw; return *this;}
        constexpr FormatInt& fill(char c){this->_fill = c; return *this;}
        constexpr FormatInt& pad_sign([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_pad_sign = b; 
            #endif
            return *this;
        }
        constexpr FormatInt& left(){this->_alignment = AlignmentLRC::Left; return *this;}
        constexpr FormatInt& right(){this->_alignment = AlignmentLRC::Right; return *this;}
        constexpr FormatInt& center(){this->_alignment = AlignmentLRC::Center; return *this;}
    };

    template<std::unsigned_integral UInt>
    std::string_view uint_to_string(char* buffer_first, char* buffer_last, UInt value, bool use_thousands_char = false, char thousands_char = ','){
        if constexpr (!std::same_as<UInt, typename embed::make_fast<UInt>::type>){
            return uint_to_string(buffer_first, buffer_last, static_cast<typename embed::make_fast<UInt>::type>(value), use_thousands_char, thousands_char);
        }else{
            char * itr = buffer_last;

            if(value == 0){
                *--itr = '0';
                return std::string_view(itr, buffer_last);
            }
    
            // convert to string
            for(size_t i = 0; value != 0 && itr != buffer_first; ++i){
                const auto mod = value % 10;
                value = value / 10;
                if((use_thousands_char!='\0') && (i != 0) && (i % 3 == 0)) {*--itr = thousands_char;};
                *--itr = '0' + static_cast<char>(mod);
            }
    
            return std::string_view(itr, buffer_last);
        }
    }

    void print_num_stringified(OStream& stream, std::string_view sign_str, std::string_view num_str, const FormatIntParams& params);

    template<std::integral Int>
        requires (std::same_as<Int, typename embed::make_fast<Int>::type>)
    void print_fast_int(OStream& stream, const Int& value, const FormatIntParams& params){
        using namespace std::string_view_literals;
        using UInt = typename std::make_unsigned<Int>::type;
        
        std::string_view sign_str = ""sv;

        UInt unsigned_value = static_cast<UInt>(value);

        if constexpr (std::is_signed_v<Int>){
            if(value < 0){
                unsigned_value = static_cast<UInt>(-value);
                sign_str = "-"sv;
                
            }else if(params._force_sign){
                sign_str = "+"sv;
            }
        }else{
            if(params._force_sign){
                sign_str = "+"sv;
            }
        }

        char buffer[32];
        const std::string_view num_str = uint_to_string(&buffer[0], &buffer[32], unsigned_value, params._use_thousands, params._thousands_char);

        print_num_stringified(stream, sign_str, num_str, params);
    }

    template<std::integral Int>
    inline void print(OStream& stream, const Int& value, const FormatIntParams& params){
        using FInt = typename embed::make_fast<Int>::type;
        const FInt fast_value(value);
        print_fast_int(stream, fast_value, params);
    }

    template<std::integral Int>
    inline OStream& operator<<(OStream& stream, const FormatInt<Int>& fvalue){
        print(stream, fvalue._value, static_cast<const FormatIntParams&>(fvalue));
        return stream;
    }
    
    template <std::integral Int>
    inline OStream& operator<<(OStream& stream, const Int& value){
        return stream << FormatInt<Int>(value);
    }

// -----------------------------------------------------------------------------------------------
//                                    integer suffix formating
// -----------------------------------------------------------------------------------------------

    struct FormatIntSuffixParams : public FormatIntParams{
        bool _pad_suffix = false;

        constexpr FormatIntSuffixParams()=default;
        constexpr FormatIntSuffixParams(const FormatIntSuffixParams&)=default;
        constexpr FormatIntSuffixParams& operator=(const FormatIntSuffixParams&)=default;

        constexpr FormatIntSuffixParams(const FormatStrParams& params) : FormatIntParams(params){}
        constexpr FormatIntSuffixParams& operator=(const FormatStrParams& params){this->FormatStrParams::operator=(params);return *this;}

        constexpr FormatIntSuffixParams(const FormatIntParams& params) : FormatIntParams(params){}
        constexpr FormatIntSuffixParams& operator=(const FormatIntParams& params){this->FormatIntParams::operator=(params);return *this;}
    };

    template<std::integral Int>
    struct FormatIntSuffix : public FormatIntSuffixParams{
    public:
        using value_type = typename embed::make_fast<Int>::type;
        value_type _value = 0;
        std::string_view _suffix = "";

        constexpr FormatIntSuffix() = default;
        constexpr FormatIntSuffix(const FormatIntSuffix&) = default;
        constexpr FormatIntSuffix& operator=(const FormatIntSuffix&) = default;

        constexpr FormatIntSuffix(Int value, std::string_view suffix) : _value(static_cast<value_type>(value)), _suffix(suffix){}
    private:
        // private constructors
        constexpr FormatIntSuffix(Int value, std::string_view suffix, const FormatStrParams& params) : FormatIntSuffixParams(params), _value(value), _suffix(suffix){}
        constexpr FormatIntSuffix(Int value, std::string_view suffix, const FormatIntParams& params) : FormatIntSuffixParams(params), _value(value), _suffix(suffix){}
        constexpr FormatIntSuffix(Int value, std::string_view suffix, const FormatIntSuffixParams& params) : FormatIntSuffixParams(params), _value(static_cast<value_type>(value)), _suffix(suffix){}
    public:

        static constexpr FormatIntSuffix like(Int value, std::string_view suffix, const FormatIntParams& params){return FormatIntSuffix(value, suffix, params);}
        static constexpr FormatIntSuffix like(Int value, std::string_view suffix, const FormatStrParams& params){return FormatIntSuffix(value, suffix, params);}
        static constexpr FormatIntSuffix like(Int value, std::string_view suffix, const FormatIntSuffixParams& params){return FormatIntSuffix(value, suffix, params);}

        /**
         * \brief adds padding between the number and the suffix
         */
        constexpr FormatIntSuffix& pad_suffix(bool b = true){
            #ifndef EMBED_FMT_MINIMAL
                this->_pad_suffix = b;
            #endif
            return *this;
        }

        /**
         * \brief enables thousand characters
         * 
         * Example:
         * ```C++
         * stream << FormatIntSuffix(4203).use_thousands(); //Outputs 4,203
         * stream << FormatIntSuffix(4203).use_thousands(true); //Outputs 4,203
         * stream << FormatIntSuffix(4203).use_thousands(false); //Outputs 4203
         * ```
         */
        constexpr FormatIntSuffix& use_thousands([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_use_thousands = b; 
            #endif
            return *this;
        }

        /**
         * \brief enables thousand characters
         * 
         * Example:
         * ```C++
         * stream << FormatIntSuffix(4203).thousands('.'); //Outputs 4.203
         * stream << FormatIntSuffix(4203).thousands(','); //Outputs 4,203
         * ```
         */
        constexpr FormatIntSuffix& thousands([[maybe_unused]]char c=','){
            #ifndef EMBED_FMT_MINIMAL
                this->_thousands_char = c; 
                this->_use_thousands = true; 
            #endif
            return *this;
        }

        /// @brief force the printing of the sign, even if it is positive. 
        constexpr FormatIntSuffix& fsign([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_force_sign = b; 
            #endif
            return *this;
        }
        constexpr FormatIntSuffix& mwidth(int mw){this->_mwidth = mw; return *this;}
        constexpr FormatIntSuffix& fill(char c){this->_fill = c; return *this;}
        constexpr FormatIntSuffix& pad_sign([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_pad_sign = b; 
            #endif
            return *this;
        }
        constexpr FormatIntSuffix& left(){this->_alignment = AlignmentLRC::Left; return *this;}
        constexpr FormatIntSuffix& right(){this->_alignment = AlignmentLRC::Right; return *this;}
        constexpr FormatIntSuffix& center(){this->_alignment = AlignmentLRC::Center; return *this;}
    };

    void print_num_stringified(OStream& stream, std::string_view sign_str, std::string_view num_str, std::string_view suffix, const FormatIntSuffixParams& params);

    template<std::integral Int>
        requires(std::same_as<Int, typename embed::make_fast<Int>::type>)
    void print_fast_int(OStream& stream, const Int& value, std::string_view suffix, const FormatIntSuffixParams& params){
        using namespace std::string_view_literals;
        using UInt = typename std::make_unsigned<Int>::type;

        std::string_view sign_str = ""sv;

        UInt unsigned_value = 0;

        if(value >= 0){
            unsigned_value = value;
            if(params._force_sign){
                sign_str = "+"sv;
            }
        }else{
            unsigned_value = static_cast<std::make_unsigned<Int>::type>(-value);
            sign_str = "-"sv;
        }

        char buffer[32];
        const std::string_view num_str = uint_to_string(&buffer[0], &buffer[32], unsigned_value, params._use_thousands, params._thousands_char);

        print_num_stringified(stream, sign_str, num_str, suffix, params);
    }

    template<std::integral Int>
    inline void print(OStream& stream, const Int& value, std::string_view suffix, const FormatIntSuffixParams& params){
        using FastInt = embed::make_fast<Int>::type;
        const FastInt fast_value(value);
        print_fast_int(stream, fast_value, suffix, params);
    }

    template<std::integral Int>
    OStream& operator<<(OStream& stream, const FormatIntSuffix<Int>& value){
        print(stream, value._value, value._suffix, static_cast<const FormatIntSuffixParams&>(value));
        return stream;
    }

// -----------------------------------------------------------------------------------------------
//                                    fload formating
// -----------------------------------------------------------------------------------------------


    /// @brief Enum to specify the formating of floating point numbers
    enum class FloatRepresentation{
        Scientific,  ///< numbers are represented, so that there is always only one digit before the comma. E.g.: 1.23, 1.23e+1, 1.23e+2, 1.23e+3
        Engineering, ///< numbers are represented with exponents in order or three. E.g.: 1.23, 12.3, 123e, 1.23e+3
        Full,        ///< numbers are represented fully without exponents. E.g.: 1.23, 12.3, 123., 1230    
    };


    struct FormatFloatParams : public FormatIntParams{
        #ifndef EMBED_FMT_MINIMAL
            FloatRepresentation _representation = FloatRepresentation::Engineering;
            char _comma = '.';
            unsigned int _decimals = 3;
            bool _force_comma = false;
            bool _force_decimals = false;
            bool _force_exponent = false;
            bool _force_exponent_sign = false;
        #else
            static constexpr FloatRepresentation _representation = EMBED_FMT_FLOAT_REP;
            static constexpr char _comma = EMBED_FMT_COMMA_CHAR;
            static constexpr unsigned int _decimals = EMBED_FMT_DECIMALS;
            static constexpr bool _force_comma = EMBED_FMT_FORCE_COMMA_VALUE;
            static constexpr bool _force_decimals = EMBED_FMT_FORCE_DECIMALS_VALUE;
            static constexpr bool _force_exponent = EMBED_FMT_FORCE_EXPONENT_VALUE;
            static constexpr bool _force_exponent_sign = EMBED_FMT_FORCE_EXPONENT_SIGN_VALUE;
        #endif

        constexpr FormatFloatParams()=default;
        constexpr FormatFloatParams(const FormatFloatParams&)=default;
        constexpr FormatFloatParams& operator=(const FormatFloatParams&)=default;

        constexpr FormatFloatParams(const FormatStrParams& params) : FormatIntParams(params){}
        constexpr FormatFloatParams& operator=(const FormatStrParams& params){
            this->FormatIntParams::operator=(params);
            return *this;
        }

        constexpr FormatFloatParams(const FormatIntParams& params) : FormatIntParams(params){}
        constexpr FormatFloatParams& operator=(const FormatIntParams& params){
            this->FormatIntParams::operator=(params);
            return *this;
        }
    };

    /**
     * \brief Formats an floating point for further printing use
     */
    struct FormatFloat : FormatFloatParams{
        using value_type = float;
        value_type _value = 0;

        constexpr FormatFloat() = default;
        constexpr FormatFloat(const FormatFloat&) = default;
        constexpr FormatFloat& operator=(const FormatFloat&) = default;

        template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
        explicit constexpr FormatFloat(Float value) : _value(static_cast<float>(value)){}

    private:
        // private constructors
        template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
        constexpr FormatFloat(Float value, const FormatFloatParams& params) : FormatFloatParams(params), _value(static_cast<float>(value)){}

        template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
        constexpr FormatFloat(Float value, const FormatIntParams& params) : FormatFloatParams(params), _value(static_cast<float>(value)){}

        template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
        constexpr FormatFloat(Float value, const FormatStrParams& params) : FormatFloatParams(params), _value(static_cast<float>(value)){}
    public:

        template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
        constexpr static FormatFloat like(Float value, const FormatFloatParams& params){return FormatFloat(value, params);}

        template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
        constexpr static FormatFloat like(Float value, const FormatIntParams& params){return FormatFloat(value, params);}

        template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
        constexpr static FormatFloat like(Float value, const FormatStrParams& params){return FormatFloat(value, params);}

        /**
         * \brief enables thousand characters
         */
        constexpr FormatFloat& use_thousands([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_use_thousands = b; 
            #endif
            return *this;
        }

        /// @brief enable thousands seperator
        /// @param c the character to be used for thousands seperator (default: ',')
        constexpr FormatFloat& thousands([[maybe_unused]]char c=','){
            #ifndef EMBED_FMT_MINIMAL
                this->_thousands_char = c;
                this->_use_thousands = true; 
            #endif
            return *this;
        }

        /// @brief sets the comma value (default: '.')
        constexpr FormatFloat& comma([[maybe_unused]]char c){
            #ifndef EMBED_FMT_MINIMAL
                this->_comma = c; 
                this->_use_thousands = true; 
            #endif
            return *this;
        }

        /// @brief forces the printing of the sign, even if it is positive
        constexpr FormatFloat& fsign([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_force_sign = b; 
            #endif
            return *this;
        }

        /// @brief sets the minimum width the the number should occupy in text - the rest will be filled with padding/fill characters 
        constexpr FormatFloat& mwidth(int mw){this->_mwidth = mw; return *this;}

        /// @brief sets the padding/fill character
        constexpr FormatFloat& fill(char c){this->_fill = c; return *this;}

        /// @brief enables padding for the sign character
        constexpr FormatFloat& pad_sign([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_pad_sign = b; 
            #endif
            return *this;
        }

        /// @brief flushright: sets the alignment to right - padding will be applied to the left
        constexpr FormatFloat& right(){this->_alignment = AlignmentLRC::Right; return *this;}

        /// @brief flushleft: sets the alignment to left - padding will be applied to the right
        constexpr FormatFloat& left(){this->_alignment = AlignmentLRC::Left; return *this;}

        /// @brief center: sets the alignment to center - padding will be applied to both sides
        constexpr FormatFloat& center(){this->_alignment = AlignmentLRC::Center; return *this;}
        
        /// @brief formats the number in scientific notation: exponent so that there is always one digit before the comma.
        constexpr FormatFloat& sci(){
            #ifndef EMBED_FMT_MINIMAL
                this->_representation = FloatRepresentation::Scientific; 
            #endif
            return *this;
        }

        /// @brief formats the number in engineering notation: exponent in magnitudes of three
        constexpr FormatFloat& eng(){
            #ifndef EMBED_FMT_MINIMAL
                this->_representation = FloatRepresentation::Engineering; 
            #endif
            return *this;
        }

        /// @brief formats the number so that it is shown in its full length without the use of exponents
        constexpr FormatFloat& full(){
            #ifndef EMBED_FMT_MINIMAL
                this->_representation = FloatRepresentation::Full; 
            #endif
            return *this;
        }

        /// @brief sets the precision, aka. the number of total digits that will be printed before and after the comma
        constexpr FormatFloat& decimals([[maybe_unused]]int d){
            #ifndef EMBED_FMT_MINIMAL
                this->_decimals = d; 
            #endif
            return *this;
        }

        /// @brief force a comma in every print, even if it is not needed to display the number
        constexpr FormatFloat& fcomma([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_force_comma = b; 
            #endif
            return *this;
        }

        /// @brief force the printing of all decimals, even if they are zero
        constexpr FormatFloat& fdeci([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_force_decimals = b; 
            #endif
            return *this;
        }

        /// @brief force the printing of the exponent, even if it is zero
        constexpr FormatFloat& fexp([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_force_exponent = b; 
            #endif
            return *this;
        }

        /// @brief force the sign of the exponent, even if it is positive
        constexpr FormatFloat& fexpsign([[maybe_unused]]bool b=true){
            #ifndef EMBED_FMT_MINIMAL
                this->_force_exponent_sign = b; 
            #endif
            return *this;
        }
    };

    struct str_add_float_params{
        FloatRepresentation representation = FloatRepresentation::Scientific;
        #ifndef EMBED_FMT_MINIMAL
            unsigned int decimals = 6;
            char comma_char = '.';
            char thousands_char = ',';
            bool use_thousands = false;
            bool force_comma = false;
            bool force_decimals = false;
            bool force_sign = false;
            bool force_exponent = false;
            bool force_exponent_sign = false;
        #else
            static constexpr unsigned int decimals = EMBED_FMT_DECIMALS;
            static constexpr char comma_char = EMBED_FMT_COMMA_CHAR;
            static constexpr char thousands_char = EMBED_FMT_THOUSANDS_CHAR;
            static constexpr bool use_thousands = EMBED_FMT_THOUSANDS_VALUE;
            static constexpr bool force_comma = EMBED_FMT_FORCE_COMMA_VALUE;
            static constexpr bool force_decimals = EMBED_FMT_FORCE_DECIMALS_VALUE;
            static constexpr bool force_sign = EMBED_FMT_FORCE_SIGN_VALUE;
            static constexpr bool force_exponent = EMBED_FMT_FORCE_EXPONENT_VALUE;
            static constexpr bool force_exponent_sign = EMBED_FMT_FORCE_EXPONENT_SIGN_VALUE;
        #endif
    };

    OStream& operator<<(OStream& stream, FormatFloat value);

    template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
    inline OStream& operator<<(OStream& stream, Float value){
        return stream << FormatFloat(value);
    }

    // -----------------------------------------------------------------------------------------------
    //                                    pointer overloads
    // -----------------------------------------------------------------------------------------------
    
    struct FormatHexParams{
        bool include_header = true;
        bool force_digits = true;
        bool lowercase = true;
    };

    template<std::unsigned_integral UInt>
    struct FormatHex : public FormatStrParams, public FormatHexParams{
        UInt _value;

        FormatHex(const UInt& v) : _value(v){}

        /**
         * @brief enable the hex header '0x' in the beginning.
         */
        constexpr FormatHex& head(){this->include_header = true; return *this;}

        /**
         * @brief disable the hex header '0x' in the beginning.
         */
        constexpr FormatHex& nohead(){this->include_header = false; return *this;}

        /**
         * @brief enable leading zeros of the hex number
         */
        constexpr FormatHex& lead(){this->force_digits = true; return *this;}

        /**
         * @brief disables leading zeros of the hex number
         */
        constexpr FormatHex& nolead(){this->force_digits = true; return *this;}

        /**
         * @brief use lower case letters for the hex number
         */
        constexpr FormatHex& lower(){this->lowercase = true; return *this;}

        /**
         * @brief use uppercase letters for hex numbers
         */
        constexpr FormatHex& upper(){this->lowercase = false; return *this;}

        
        /**
         * \brief Sets the minimum number of character that will be put into the stream. 
         * 
         * Padded characters are defined by FormatStr::fill(char c). 
         * The default fill character is a space ' '.
         * 
         * Example:
         * ```C++
         * stream << "[" << FormatStr("OK").mwidth(6) << "]" << endl; // Outputs: [    OK]
         * ```
         * 
         * \param mw An integer that sets the new minimal width
         * \returns self
         */
        constexpr FormatHex& mwidth(int mw){this->_mwidth = mw; return *this;}

        /**
         * \brief Formats the string to the left area set by mwidth().
         * 
         * Example:
         * ```C++
         * stream << "[" << FormatStr("OK").mwidth(6).left() << "]" << endl; // Outputs: [OK    ]
         * ```
         * 
         * \returns self
         */
        constexpr FormatHex& left(){this->_alignment = AlignmentLRC::Left; return *this;}

        /**
         * \brief Formats the string to the left area set by mwidth().
         * 
         * Example:
         * ```C++
         * stream << "[" << FormatStr("OK").mwidth(6).right() << "]" << endl; // Outputs: [    OK]
         * ```
         * 
         * \returns self
         */
        constexpr FormatHex& right(){this->_alignment = AlignmentLRC::Right; return *this;}

        /**
         * \brief Formats the string to the left area set by mwidth().
         * 
         * Example:
         * ```C++
         * stream << "[" << FormatStr("OK").mwidth(6).center() << "]" << endl; // Outputs: [  OK  ]
         * ```
         * 
         * \returns self
         */
        constexpr FormatHex& center(){this->_alignment = AlignmentLRC::Center; return *this;}

        /**
         * \brief Sets the fill characters used by padding that is applied when mwidth() is used.
         * \param c The new fill character
         * \returns self
         */
        constexpr FormatHex& fill(char c){this->_fill = c; return *this;}

    };

    template<std::unsigned_integral UInt>
    OStream& operator<<(OStream& stream, FormatHex<UInt> hex){
        char buffer[sizeof(hex)*2];
        char* itr = &buffer[0];

        if(hex.include_header){
            *itr++ = '0';
            *itr++ = 'x';
        }

        // print at least 1 zero
        if(hex._value == 0 && !hex.force_digits){
            *itr++ = '0';
            return stream << FormatStr::like(buffer, itr, hex);
        }

        int i = 0;

        // optional skipp zeros
        if(!hex.force_digits){
            for(; i < 8; ++i){
                uint32_t h = (hex._value >> (4 * (7 - i))) & 0xF;
                if(h != 0){
                    break;
                }
            }
        }

        // hex tables
        constexpr char hex_upper[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        constexpr char hex_lower[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        // print hex characters
        for(; i < 8; ++i){
            uint32_t h = (hex._value >> (4 * (7 - i))) & 0xF;
            *itr++ = (hex.lowercase) ? hex_lower[h] : hex_upper[h];
        }

        // output the formated string
        return stream << FormatStr::like(buffer, itr, hex);
    }

    // -----------------------------------------------------------------------------------------------
    //                                    pointer overloads
    // -----------------------------------------------------------------------------------------------

    template<class T>
    inline OStream& operator<<(OStream& stream, const T* ptr){
        return stream << FormatHex(reinterpret_cast<std::uintptr_t>(ptr)); // TODO: format as hex
    }

    
    inline OStream& operator<<(OStream& stream, [[maybe_unused]]std::nullptr_t ptr){
        return stream << "nullptr";
    }

// -----------------------------------------------------------------------------------------------
    //                                    chrono overloads
    // -----------------------------------------------------------------------------------------------

    template<class Rep, class Period = std::ratio<1, 1>>
    auto format_chrono(std::chrono::duration<Rep, Period> duration){
        if constexpr (std::ratio_greater_equal<Period, typename std::chrono::years::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::years>(duration).count(), "Y");
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::months::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::months>(duration).count(), "M");
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::weeks::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::weeks>(duration).count(), "W");
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::days::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::day>(duration).count(), "D");
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::hours::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::hours>(duration).count(), "h");
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::minutes::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::minutes>(duration).count(), "m");
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::seconds::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::seconds>(duration).count(), "s");
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::milliseconds::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::milliseconds>(duration).count(), "ms");
        }else if constexpr (std::ratio_greater_equal<Period, typename std::chrono::microseconds::period>::value){
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::microseconds>(duration).count(), "us");
        }else /* if constexpr (std::ratio_greater_equal<Period, typename std::chrono::nanoseconds::period>::value) */ {
            return FormatIntSuffix(embed::rounding_duration_cast<std::chrono::nanoseconds>(duration).count(), "ns");
        }
    }

    template<class Rep, class Period = std::ratio<1, 1>>
    OStream& operator<<(OStream& stream, std::chrono::duration<Rep, Period> duration){
        return stream << format_chrono(duration);
    }

    // -----------------------------------------------------------------------------------------------
    //                                    reverse_iterator overloads
    // -----------------------------------------------------------------------------------------------

    template<class T>
    OStream& operator<<(OStream& stream, std::reverse_iterator<T> value){
        return stream << "std::reverse_iterator<>(" << (value.base()-1) << ")";
    }

    // -----------------------------------------------------------------------------------------------
    //                           Stream References and Default Streams
    // -----------------------------------------------------------------------------------------------

    struct OStreamRef{
        OStream* ptr = nullptr;

        constexpr OStreamRef() = default;
        
        constexpr OStreamRef(const OStreamRef&)=default;
        explicit constexpr OStreamRef(OStream& stream) : ptr(&stream){}
        explicit constexpr OStreamRef(OStream* stream) : ptr(stream){}

        constexpr OStreamRef& operator=(const OStreamRef&)=default;
        constexpr OStreamRef& operator=(OStream& stream){this->ptr = &stream; return *this;}
        constexpr OStreamRef& operator=(OStream* stream){this->ptr = stream; return *this;}

        constexpr bool is_open() const {return this->ptr != nullptr;}

        constexpr operator bool() const {return this->is_open();}

    };

    template<class T>
    inline OStream& operator<<(OStreamRef stream, const T& value){
        EMBED_ASSERT_O1_MSG(stream.is_open(), "No output stream! S: Assign an output stream to `embed::cout`, `embed::cerr` or `embed::clog`.");
        return (*(stream.ptr)) << value;
    }

    extern OStreamRef cout;
    extern OStreamRef cerr;
    extern OStreamRef clog;

}// namespace: embed
