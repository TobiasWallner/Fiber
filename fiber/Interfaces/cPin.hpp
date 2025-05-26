#pragma once

#include "cAwaitable.hpp"

#include <concepts>
#include <bitset>

namespace fiber
{

    // ------------------ single pin ------------------

    /**
     * \brief Concept for a pin that can be immediatly written to
     * 
     * Pin without any practical delay, like pins of the micro controller
     * 
     * ---
     * ```cpp
     * void set(bool b);
     * ```
     * 
     * sets the pin output level to the passed state
     * 
     * - *param b*: the new state for the pin
     *      - `true`: logic high
     *      - `false`: logic low
     * 
     * ---
     * 
     * ```cpp
     * void high();
     * ```
     * sets the pin to logic high
     * 
     * ---
     * 
     * ```cpp
     * void low();
     * ```
     * sets the pin to logic low
     * 
     * ---
     * 
     * ```cpp
     * bool read();
     * ```
     * 
     * reads the value from the pin
     * 
     * *Returns:*  the logic value applied to the pin.
     */
    template<class Pin>
    concept cOutPin = requires(Pin pin, bool level){
        { pin.set(level) } -> std::same_as<void>;
        { pin.high() } -> std::same_as<void>;
        { pin.low() } -> std::same_as<void>;
        { pin.read() } -> std::same_as<bool>;
    };

    /**
     * \brief Concept for pin that can be immediatelly read from
     * 
     * Pin without any practical delay, like pins of the micro controller
     * 
     * ---
     * 
     * ```cpp
     * bool read();
     * ```
     * 
     * reads the value from the pin
     * 
     * *Returns:*  the logic value applied to the pin.
     */
    template<class Pin>
    concept cInPin = requires(Pin pin){
        { pin.read() } -> std::same_as<bool>;
    };

    /**
     * \brief Concept for tri-state pin (high, low, input) that can be immediatelly read from and written to
     * 
     * For pin without any practical delay, like pins of the micro controller.
     * 
     * Implements the following interface:
     * 
     * ---
     * ```cpp
     * void set(bool b);
     * ```
     * 
     * sets the pin output level to the passed state
     * 
     * - *param b*: the new state for the pin
     *      - `true`: logic high
     *      - `false`: logic low
     * 
     * ---
     * 
     * ```cpp
     * void high();
     * ```
     * sets the pin to logic high
     * 
     * ---
     * 
     * ```cpp
     * void low();
     * ```
     * sets the pin to logic low
     * 
     * ---
     * 
     * ```cpp
     * void dir(bool b);
     * ```
     * 
     * sets the pin direction to the passed state
     * 
     * - *param b*: the new direction for the pin
     *      - `true`: input
     *      - `false`: output
     * 
     * ---
     * 
     * ```cpp
     * void input();
     * ```
     * 
     * configurates the pin as an input-pin that can be read from.
     * 
     * ---
     * 
     * ```cpp
     * void output();
     * ```
     * 
     * configurates the pin as an output pin that can be written to
     * 
     * ---
     * 
     * ```cpp
     * bool read();
     * ```
     * 
     * reads the value from the pin
     * 
     * *Returns:*  the logic value applied to the pin.
     */
    template<class Pin>
    concept cPin = requires(Pin pin, bool level){
        { pin.high() } -> std::same_as<void>;
        { pin.low() } -> std::same_as<void>;
        { pin.set(level) } -> std::same_as<void>;
        { pin.input() } -> std::same_as<void>;
        { pin.read() } -> std::same_as<bool>;
    };
s
    // ------------------ multiple pins ------------------

    /**
     * \brief Concept for synchronous output pins
     * 
     * Pins without any practical delay, like pins of the micro controller
     */
    template<class Pins, std::size_t N>
    concept cOutPins = requires(Pins pins, std::bitset<N> values, std::size_t n){
        { pins.set(values) } -> std::same_as<void>;
        { pins.view(0) } -> cOutPin;
    };

    /**
     * \brief Concept for synchronous output pins
     * 
     * Pins without any practical delay, like pins of the micro controller
     */
    template<class Pins, std::size_t N>
    concept cInPins = requires(Pins pins){
        { pins.read() } -> std::same_as<std::bitset<N>>;
        { pins.view(0) } -> cInPin;
    };

    /**
     * \brief Concept for tri-state pin (high, low, input) that can be immediatelly read from and written to
     * 
     * Pin without any practical delay, like pins of the micro controller
     * 
     * The `set()` function writes levels to output pins and ignores values for input pins, where:
     *   - `true`, `1` --> High
     *   - `false`, `0` --> Low
     * 
     * The `dir()` function specifies the input/output direction of the pin, where:
     *   - `true`, '1' --> Input
     *   - `false`, '0' --> Output
     * 
     */
    template<class Pins, std::size_t N>
    concept cPins = requires(Pins pins, std::bitset<N> values){
        { pins.set(values) } -> std::same_as<void>;
        { pins.dir(values) } -> std::same_as<void>;
        { pins.read() } -> std::same_as<std::bitset<N>>;
        { pins.view(0) } -> cPin;
    };

// ---------------------------------------------------------------------------------------------
//                                  Helpers
// ---------------------------------------------------------------------------------------------

    /**
     * \brief Represents an unconnected pin that does nothing
     */
    class VoidPin{
        public:

        /// @brief sets the pin output level to the passed state
        /// @param b the new state for the pin
        ///   - `true`: logic high
        ///   - `false`: logic low
        constexpr void set([[maybe_unused]]bool b){}

        /// @brief sets the pin to logic high
        constexpr void high(){}

        /// @brief sets the pin to logic low
        constexpr void low(){}

        /// @brief sets the pin direction to the passed state
        /// @param b the new direction for the pin
        ///   - `true`: input
        ///   - `false`: output
        constexpr void dir([[maybe_unused]] bool b){}

        /// @brief configurates the pin as an input pin that can be read from
        constexpr void input(){}

        /// @brief configurates the pin as an output pin that can be written to
        constexpr void output(){}

        /// @brief reads the value from the pin
        /// @return the logic value from the pin
        constexpr bool read(){}
    }

} // namespace fiber
