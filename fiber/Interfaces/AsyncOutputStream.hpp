#pragma once

// std
#include <concepts>
#include <cstdint>

// fiber
#include "Writeable.hpp"
#include "Awaitable.hpp"

namespace fiber
{

    /// @brief Interface for output streams. Allows to write single values and sequences of values.
    /// @tparam ValueType Value that can be written to the stream
    template<class ValueType>
    class iAsyncOutputStream : public iWritable<ValueType>{
        public:

        virtual ~iOutputStream(){}
        
        /**
         * \brief Writes an array of data to the stream 
         * \param array a pointer to the start of the arrary
         * \param len the length of the array
         */
        virtual iAwaitable<void> async_write(ValueType const * array, size_t len) = 0;

        /**
         * \brief Overload this method to flush flush the buffer
         */
        virtual iAwaitable<void> async_flush() = 0;
    };

    /**
     * \brief Concept for output streams. Allows to write single values and sequences of values.
     * \tparam OutputStream The output stream object
     * \tparam ValueType Value that can be written to the stream
     */
    template<cWritable OutputStream, class ValueType>
    concept cAsyncOutputStream = requires(OutputStream ostream, ValueType const * value, size_t len){
        { ostream.async_write(value, len) } -> std::same_as<cAwaitable<void>>;
        { ostream.async_flush(value, len) } -> std::same_as<cAwaitable<void>>;
    };
} // namespace fiber
