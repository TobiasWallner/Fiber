#pragma once

#include <concepts>

namespace fiber
{
    /// @brief An interface for something that can be read from
    /// @details Intended to be used for things with a value that is always readable like a GPIO (General Purpose Input Output) Pin
    /// @tparam ValueType The value type that can be read
    template<class ValueType>
    class iReadable{
        public:
        virtual ~iReadable(){};

        /**
         * \brief reads from a peripheral without changeing the state of the peripheral
         * 
         * If you need to read function that does change the state of the peripheral use `fiber::iInputStream` instead.
         * 
         * \returns The an object of type `ValueType`.
         */
        virtual [[nodiscard]] ValueType read() const = 0;
    };

    /**
     * \brief A concept for something that can be read from
     * \tparam Writable An Object that implements the `.read()` method
     * \tparam ValueType The type of the value that can be read from the readable
     */
    template<class Readable, class ValueType>
    concept cReadable = requires(Readable readable, ValueType value){
        { readable.read() } -> std::same_as<ValueType>;
    };
    
} // namespace fiber

