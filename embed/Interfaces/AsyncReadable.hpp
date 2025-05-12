#pragma once

// std
#include <concepts>

// embed
#include "Awaitable.hpp"

namespace embed
{
    /// @brief An interface for something that can be read from asynchronously 
    /// @details Intended to be used for things that expect a value in the future, like reading from a ADC (Analog to Digital Converter)
    /// @tparam ValueType The value type that can be read
    template<class ValueType>
    class iAsyncReadable{
        public:
        virtual ~iReadable(){};

        /**
         * \brief reads from a peripheral without changeing the state of the peripheral
         * 
         * If you need to read function that does change the state of the peripheral use `embed::iInputStream::get()` instead.
         * 
         * \returns The an object of type `ValueType`.
         */
        virtual [[nodiscard]] iAwaitable<ValueType> async_read() = 0;
    };

    /**
     * \brief A concept for something that can be read from
     * \tparam Writable An Object that implements the `.read()` method
     * \tparam ValueType The type of the value that can be read from the readable
     */
    template<class AsyncReadable, class ValueType>
    concept cAsyncReadable = requires(AsyncReadable async_readable, ValueType value){
        { async_readable.async_read() } -> cAwaitable<ValueType>;
    };
    
} // namespace embed

