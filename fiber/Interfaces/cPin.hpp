#pragma once

#include "cAwaitable.hpp"

#include <concepts>
#include <bitset>
#include <tuple>

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
     * void write(bool b);
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
     * 
     * reads the value from the pin
     * 
     * *Returns:*  the logic value applied to the pin.
     */
    template<class Pin>
    concept cOutPin = requires(Pin pin, bool level){
        { pin.high() } -> std::same_as<void>;
        { pin.low() } -> std::same_as<void>;
        { pin.write(level) } -> std::same_as<void>;
    };

    /**
     * \brief Concept for pin that can be immediatelly read from
     * 
     * Pin without any practical delay, like pins of the micro controller
     * 
     * ---
     * 
     * ```cpp
     * [[nodiscard]] bool read() ;
     * ```
     * 
     * *Returns:*  the logic value applied to the pin.
     * 
     * ---
     * 
     * ```cpp
     * [[nodiscard]] bool is_high() ;
     * ```
     * 
     * *Returns:*  `true` is the pin is set to logic high.
     * 
     * ---
     * 
     * ```cpp
     * [[nodiscard]] bool is_low() ;
     * ```
     * 
     * *Returns:*  `true` is the pin is set to logic low.
     */
    template<class Pin>
    concept cInPin = requires( Pin pin){
        { /*nodiscard*/ pin.is_high() } -> std::same_as<bool>;
        { /*nodiscard*/ pin.is_low() } -> std::same_as<bool>;
        { /*nodiscard*/ pin.read() } -> std::same_as<bool>;
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
     * void write(bool b);
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
     * [[nodiscard]] bool read() ;
     * ```
     * 
     * *Returns:*  the logic value applied to the pin.
     * 
     * ---
     * 
     * ```cpp
     * [[nodiscard]] bool is_high() ;
     * ```
     * 
     * *Returns:*  `true` is the pin is set to logic high.
     * 
     * ---
     * 
     * ```cpp
     * [[nodiscard]] bool is_low() ;
     * ```
     */
    template<class Pin>
    concept cPin = requires(Pin pin, bool boolean){
        { pin.high() } -> std::same_as<void>;
        { pin.low() } -> std::same_as<void>;
        { pin.write(boolean) } -> std::same_as<void>;

        { pin.input() } -> std::same_as<void>;
        { pin.output() } -> std::same_as<void>;
        { pin.dir(boolean) } -> std::same_as<void>;

        { /*nodiscard*/ pin.is_high() } -> std::same_as<bool>;
        { /*nodiscard*/ pin.is_low() } -> std::same_as<bool>;
        { /*nodiscard*/ pin.read() } -> std::same_as<bool>;
    };

    // ------------------ multiple pins ------------------

    /**
     * \brief Concept for synchronous output pins
     * 
     * Pins without any practical delay, like pins of the micro controller
     */
    template<class Pins, std::size_t N>
    concept cOutPins = requires(Pins pins, std::bitset<N> values, std::size_t n){
        { pins.write(values) } -> std::same_as<void>;
        { pins.template view<0>() } -> cOutPin;
    };

    /**
     * \brief Concept for synchronous output pins
     * 
     * Pins without any practical delay, like pins of the micro controller
     */
    template<class Pins, std::size_t N>
    concept cInPins = requires(Pins pins){
        { pins.read() } -> std::same_as<std::bitset<N>>;
        { pins.template view<0>() } -> cInPin;
    };

    /**
     * \brief Concept for tri-state pin (high, low, input) that can be immediatelly read from and written to
     * 
     * Pin without any practical delay, like pins of the micro controller
     * 
     * The `write()` function writes levels to output pins and ignores values for input pins, where:
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
        { pins.write(values) } -> std::same_as<void>;
        { pins.dir(values) } -> std::same_as<void>;
        { pins.read() } -> std::same_as<std::bitset<N>>;
        { pins.template view<0>() } -> cPin;
    };

// ---------------------------------------------------------------------------------------------
//                                  Helpers
// ---------------------------------------------------------------------------------------------

    /**
     * \brief Represents an unconnected pin that does nothing
     */
    class VoidPin{
        public:

        /// @brief does nothing (no-op)
        constexpr void write([[maybe_unused]]bool b){}

        /// @brief does nothing (no-op)
        constexpr void high(){}

        /// @brief does nothing (no-op)
        constexpr void low(){}

        /// @brief does nothing (no-op)
        constexpr void dir([[maybe_unused]] bool b){}

        /// @brief does nothing (no-op)
        constexpr void input(){}

        /// @brief does nothing (no-op)
        constexpr void output(){}

        /// @brief always returns false
        [[nodiscard]] constexpr bool read()  {return false;}

        /// @brief always returns false
        [[nodiscard]] constexpr bool is_high()  {return false;}

        /// @brief always returns true
        [[nodiscard]] constexpr bool is_low()  {return true;}
    }; static_assert(cPin<VoidPin>, "VoidPin needs to implement cPin");

    template<cPin Pin>
    class InvertedPin{
    private:
        Pin _pin;

    public: 
        constexpr InvertedPin(const Pin& pin) : _pin(pin){}

        constexpr void high(){_pin.low();}
        constexpr void low(){_pin.high();}
        constexpr void write(bool level){_pin.write(!level);}

        constexpr void input(){_pin.input();}
        constexpr void output(){_pin.output();}
        constexpr void dir(bool direction){_pin.dir(direction);}
        
        [[nodiscard]] constexpr bool is_high()  {return _pin.is_low();}
        [[nodiscard]] constexpr bool is_low()  {return _pin.is_high();}
        [[nodiscard]] constexpr bool read()  {return !_pin.read();}
    }; static_assert(cPin<InvertedPin<VoidPin>>, "InvertedPin needs to implement cPin");

    template<size_t N>
    class VoidPins{
    public:
        /// @brief does nothing (no-op)
        constexpr void write([[maybe_unused]]std::bitset<N> values){}

        /// @brief does nothing (no-op)
        constexpr void dir([[maybe_unused]]std::bitset<N> values){}
        
        /// @brief does nothing (no-op) and returns `0`
        [[nodiscard]] constexpr std::bitset<N> read(){return std::bitset<N>(0);}
        
        /// @brief returns a void pin 
        template<size_t pin>
        requires(pin < N)
        [[nodiscard]] constexpr VoidPin view(){return VoidPin();}
        
    }; static_assert(cPins<VoidPins<8>, 8>, "cPinArray needs to implement cPins");

    /**
     * \brief A tuple (heterogenious collection) of pins that can be written together as if it was a port
     */
    template<typename... Pins>
    requires (cPin<Pins> && ...)
    class PinTuple{
        std::tuple<Pins...> pins;

    public:
        
        /**
         * \brief constructs a PinTuple from a list of pins
         */
        constexpr PinTuple(Pins... pins) : pins(std::forward<Pins>(pins)...){}

        /**
         * \brief writes each bit to each corresponding pin
         */
        constexpr void write(std::bitset<sizeof...(Pins)> values){
            write_impl(values, std::make_index_sequence<sizeof...(Pins)>{});
        }

        /**
         * \brief writes each bit to configure each corresponding pin
         */
        constexpr void dir(std::bitset<sizeof...(Pins)> values){
            dir_impl(values, std::make_index_sequence<sizeof...(Pins)>{});
        }

        /**
         * \brief reads from all pins and writes their results to the corresponding bit position
         */
        [[nodiscard]] constexpr std::bitset<sizeof...(Pins)> read()  {
            return read_impl(std::make_index_sequence<sizeof...(Pins)>{});
        }

        /**
         * \brief returns a view of the corresponding pin position
         */
        template<size_t pin>
        requires(pin < sizeof...(Pins))
        [[nodiscard]] constexpr auto view(){return std::get<pin>(pins);}

    private:
        template<std::size_t... I>
        constexpr void write_impl(const std::bitset<sizeof...(Pins)>& values, std::index_sequence<I...>) {
            (..., std::get<I>(pins).write(values[I]));
        }

        template<std::size_t... I>
        constexpr void dir_impl(const std::bitset<sizeof...(Pins)>& values, std::index_sequence<I...>) {
            (..., std::get<I>(pins).dir(values[I]));
        }

        template<std::size_t... I>
        constexpr std::bitset<sizeof...(Pins)> read_impl(std::index_sequence<I...>)  {
            std::bitset<sizeof...(Pins)> result = ((std::bitset<sizeof...(Pins)>(std::get<I>(pins).read() ? 1 : 0) << I) | ...);
            return result;
        }

    }; static_assert(cPins<PinTuple<VoidPin, VoidPin>, 2>, "PinTuple needs to implement cPins");

} // namespace fiber
