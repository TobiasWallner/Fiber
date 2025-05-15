#pragma once

// std
#include <chrono>
#include <variant>

// fiber
#include <fiber/Chrono/Duration.hpp>
#include <fiber/Exceptions/Exceptions.hpp>

namespace fiber
{

    /**
     * @brief A signal that can be sent from an awaitable to the Task and Scheduler
     * 
     * Enables stuff like:
     * ```
     * co_await Delay(5ms)
     * ```
     * and internally Delay sends an `AwaitableNode::await_suspend_signal()`.
     * 
     * @see AwaitableNode
     * @see Delay
     * @see NextCycle
     */
    class CoSignal{
    public:    

        /// @brief Type of the contained signal
        enum class Type{None = 0, Await, NextCycle, ImplicitDelay, ExplicitDelay};

    private:
        fiber::Duration _delay = fiber::Duration(0);
        fiber::Duration _deadline = fiber::Duration(0);
        Type _type = Type::None;

    public:
        /// @brief Send no signal to the Task/Scheduler or clear the previous one 
        constexpr CoSignal& none(){this->_type = Type::None; return *this;}

        /// @brief Send an await signal to the Task/Scheduler. The Coroutine is waiting on an external event (Hardware/IO/other task/etc.) 
        constexpr CoSignal& await(){this->_type = Type::Await; return *this;}

        /// @brief Send the completion of this cycle and trigger the recalculation of the next one 
        constexpr CoSignal& next_cycle(){this->_type = Type::NextCycle; return *this;}

        /// @brief Suspend execution and delay the next schedule
        /// @param delay time in ns relative from now
        constexpr CoSignal& implicit_delay(fiber::Duration delay){
            this->_delay = delay;
            this->_type = Type::ImplicitDelay;
            return *this;
        }

        /// @brief Suspend execution and delay the next schedule
        /// @param delay time in ns relative to now
        /// @param rel_deadline time in ns relative to the resulting ready time
        constexpr CoSignal& explicit_delay(fiber::Duration delay, fiber::Duration rel_deadline){
            this->_delay = delay;
            this->_deadline = rel_deadline;
            this->_type = Type::ExplicitDelay;
            return *this;
        }

        /// @brief Get the signal type
        /// @returns An enum Type 
        constexpr Type type() const {return this->_type;}

        /// @brief get the implicit delay time
        /// @returns the implicit delay time
        /// @throws If `ASSERTION_LEVEL_O1` or higher is enabled: throws an AssertionFailure, if the signal does not hold an implicit delay
        constexpr fiber::Duration delay() const {return this->_delay;}

        /// @brief get the explicit delay time
        /// @returns the implicit delay time
        /// @throws If `ASSERTION_LEVEL_O1` or higher is enabled: throws an AssertionFailure, if the signal does not hold an explicit delay
        constexpr fiber::Duration deadline() const {return this->_deadline;}
        
    };

    OStream& operator << (OStream& stream, fiber::CoSignal::Type type);
    
} // namespace fiber


    