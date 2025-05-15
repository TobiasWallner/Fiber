#pragma once

// std
#include <concepts>

// fiber
#include "AsyncReadable.hpp"
#include "Awaitable.hpp"



namespace fiber
{
    
    /// @brief An interface that allows async read access from streams
    /// @tparam ValueType 
    template<class ValueType>
    class iAsyncInputStream : public iAsyncReadable{
        public:

        /// @brief Reads a single value from the stream asycrunously (aka. withou blocking the current thread) and removes it from the stream buffer
        /// @return Returns an `fiber::Future` that holds a placeholder for a value that will be written in the future. May be `co_await`ed
        virtual [[nodiscard]] iAwaitable<ValueType> async_get() = 0;

        /// @brief Reads a single value from the stream asycrunously (aka. withou blocking the current thread) without removing it from the stream buffer
        /// @return Returns an `fiber::Future` that holds a placeholder for a value that will be written in the future. May be `co_await`ed 
        constexpr [[nodiscard]] iAwaitable<ValueType> async_peek() {return this->iAsyncReadable::async_read(); };

        /// @brief Reads a sequence of values from the stream asyncronously (aka. without blocking the current thread)
        /// @param array A pointer to the array/buffer where the read values should be written to
        /// @param array_length The size of the array. Maximum number of values that can be written to the array.
        /// @return Returns an `fiber::Future` that holds a placeholder for a value that will be written in the future. May be `co_await`ed
        virtual [[nodiscard]] iAwaitable<size_t> async_read(ValueType* array, size_t array_length) = 0;

        /// @brief Reads a sequence of readyly available values from the stream
        /// @details This action will remove those values from the stream
        /// @param array A pointer to tha value array that will be written to
        /// @param array_length The length of the value array. This marks the maximum number of characters that will be read and written to the array.
        /// @return The number of values that have been read.
        virtual [[nodiscard]] size_t read_some(ValueType* array, size_t array_length) = 0;
    };

    template<class AsyncInputStream, class ValueType>
    class cAsyncInputStream = requires(AsyncInputStream stream, ValueType* array, size_t array_len){
        { stream.async_get() } -> std::same_as<cAwaitable<ValueType>>;
        { stream.async_peek() } -> std::same_as<cAwaitable<ValueType>>;
        { stream.async_read(array, array_len) } -> std::same_as<cAwaitable<size_t>>;
        { stream.read_some(array, array_length) } -> std::same_as<size_t>;
    }
} // namespace fiber
