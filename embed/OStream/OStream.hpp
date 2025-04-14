#pragma once

//std
#include <cstdint>
#include <type_traits>
#include <cmath>
#include <chrono>

//embed
#include <embed/Math/math.hpp>
#include <embed/core/concepts.hpp>

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
    
    enum class AlignmentLRC{Left, Right, Center};

    struct FormatStrParams{
        AlignmentLRC _alignment = AlignmentLRC::Right;
        int _mwidth = -1;
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
         * \brief enables formating to text
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

    struct FormatIntParams : public FormatStrParams{
        #ifndef EMBED_FMT_MINIMAL
            char _thousands_char = ',';
            bool _force_sign = false;
            bool _pad_sign = false;
            bool _use_thousands = false;
        #else
            static constexpr char _thousands_char = EMBED_FMT_THOUSANDS_CHAR;
            static constexpr bool _force_sign = EMBED_FMT_FORCE_SIGN_VALUE;
            static constexpr bool _pad_sign = EMBED_FMT_PAD_SIGN_VALUE;
            static constexpr bool _use_thousands = EMBED_FMT_THOUSANDS_VALUE;
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
    struct FormatInt : FormatIntParams{
        using value_type = int_fast32_t;
        
        value_type _value = 0;

        constexpr FormatInt() = default;
        constexpr FormatInt(const FormatInt&) = default;
        constexpr FormatInt& operator=(const FormatInt&) = default;

        constexpr FormatInt(value_type value) : _value(value){}

        template <typename Int, typename std::enable_if<std::is_integral<Int>::value, int>::type = 0>
        constexpr explicit FormatInt(Int value) : _value(static_cast<value_type>(value)){}
    private:
        // private constructors
        constexpr FormatInt(value_type value, const FormatIntParams& params) : FormatIntParams(params), _value(value){}

        template <typename Int, typename std::enable_if<std::is_integral<Int>::value, int>::type = 0>
        constexpr FormatInt(Int value, const FormatStrParams& params) : FormatStrParams(params), _value(static_cast<value_type>(value)){}
    public:

        static constexpr FormatInt like(value_type value, const FormatIntParams& params){return FormatInt(value, params);}

        template <typename Int, typename std::enable_if<std::is_integral<Int>::value, int>::type = 0>
        static constexpr FormatInt like(Int value, const FormatStrParams& params){return FormatInt(value, params);}

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

    struct str_add_uint_params{
        #ifndef EMBED_FMT_MINIMAL
            char thousands_char = ',';
            bool use_thousands = false;
        #else
            static constexpr char thousands_char = EMBED_FMT_THOUSANDS_CHAR;
            static constexpr bool use_thousands = EMBED_FMT_THOUSANDS_VALUE;
        #endif
        bool force_sign = false;
    };

    char* str_add_uint(char * first, char const * last, std::make_unsigned_t<FormatInt::value_type> value, const str_add_uint_params& params);
    char* str_add_sint(char * first, char const * last, FormatInt::value_type value, const str_add_uint_params& params);

    /**
     * \brief Prints integers to streams with a certain format
     */
    OStream& operator<<(OStream& stream, const FormatInt& fvalue);
    
    template <typename Int, typename std::enable_if<std::is_integral<Int>::value, int>::type = 0>
    inline OStream& operator<<(OStream& stream, const Int& value){
        return stream << FormatInt(static_cast<FormatInt::value_type>(value));
    }


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
        constexpr FormatFloat(Float value, const FormatIntParams& params) : FormatIntParams(params), _value(static_cast<float>(value)){}

        template <typename Float, typename std::enable_if<std::is_floating_point<Float>::value, int>::type = 0>
        constexpr FormatFloat(Float value, const FormatStrParams& params) : FormatIntParams(params), _value(static_cast<float>(value)){}
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

    template<class T>
    inline OStream& operator<<(OStream& stream, const T* ptr){
        return stream << reinterpret_cast<std::size_t>(ptr); // TODO: format as hex
    }

    // -----------------------------------------------------------------------------------------------
    //                                    chrono overloads
    // -----------------------------------------------------------------------------------------------

    OStream& operator<<(OStream& stream, std::chrono::nanoseconds value);
    OStream& operator<<(OStream& stream, std::chrono::microseconds value);
    OStream& operator<<(OStream& stream, std::chrono::milliseconds value);
    OStream& operator<<(OStream& stream, std::chrono::seconds value);
    OStream& operator<<(OStream& stream, std::chrono::minutes value);

#if __cplusplus >= 202002L

    OStream& operator<<(OStream& stream, std::chrono::days value);
    OStream& operator<<(OStream& stream, std::chrono::weeks value);
    OStream& operator<<(OStream& stream, std::chrono::months value);
    OStream& operator<<(OStream& stream, std::chrono::years value);

#endif

    // -----------------------------------------------------------------------------------------------
    //                           Stream References and Default Streams
    // -----------------------------------------------------------------------------------------------

    struct OStreamRef{
        OStream* ptr = nullptr;

        constexpr OStreamRef() = default;
        
        constexpr OStreamRef(const OStreamRef&)=default;
        constexpr OStreamRef(OStream& stream) : ptr(&stream){}
        constexpr OStreamRef(OStream* stream) : ptr(stream){}

        constexpr OStreamRef& operator=(const OStreamRef&)=default;
        constexpr OStreamRef& operator=(OStream& stream){this->ptr = &stream; return *this;}
        constexpr OStreamRef& operator=(OStream* stream){this->ptr = stream; return *this;}

        inline operator OStream&(){
            if(this->ptr == nullptr){
                while(true){/* trap */}
                // You are trapped here, because you have not redirected the output streams:
                //      - embed::cout
                //      - embed::cerr
                //      - embed::clog
                //
                // Solution:
                // ---------
                // 
                //  ```
                //  class MyStream : public OStream{
                //      ...
                //  };
                // 
                //  MyStream my_stream;
                // 
                //  int main(){
                //      embed::cout = my_stream;    
                //      ...
                //      embed::cout << "hallo world" << embed::endl;
                //  }
                //  ```
                //

            }else{
                return *this->ptr;
            }
        }
    };

    extern OStreamRef cout;
    extern OStreamRef cerr;
    extern OStreamRef clog;

}// namespace: embed
