#pragma once

#include <concepts>

namespace fiber
{
    /// @brief An interface for something that can be written to
    /// @tparam ValueType The value type that can be written
    template<class ValueType>
    class iWritable{
        public:
        ~iWritable(){};
        void write(const T&) = 0;
    };

    inline 

    /**
     * \brief A concept for something that can be written to
     * \tparam Writable An Object that implements the `.write(value)` method
     * \tparam ValueType The type of the value that can be written to the writeable
     */
    template<class Writable, class ValueType>
    concept cWritable = requires(Writable writable, ValueType value){
        { writable.write(value) } -> std::same_as<void>;
    };
    
} // namespace fiber

