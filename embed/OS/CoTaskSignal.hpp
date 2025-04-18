#pragma once

// std
#include <chrono>
#include <variant>

// embed
#include <embed/Exceptions/Exceptions.hpp>

namespace embed
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
    class CoTaskSignal{
    public:    

        /// @brief Type of the contained signal
        enum class Type{None = 0, Await, NextCycle, ImplicitDelay, ExplicitDelay};

    private:
        /// @brief Symbolises the absense of a signal
        struct None{};

        /// @brief Symbolises a normal await --> signals to the scheduler that this task will suspend and be put to the await-queue (aka. auxilary queue).
        struct Await{};

        /// @brief Symbolises that the task finished a cycle and the task calculates its next schedule. The scheduler will put this task then back into the waiting queue.
        struct NextCycle{};

        /// @brief Symbolises an implicit delay where the scheduler calculates the next schedule and the dealine is infered from the last schedule. The scheduler will then put this task back into the waiting queue.
        struct ImplicitDelay{std::chrono::nanoseconds delay;};

        /// @brief Symbolises and explicit delay from now with a deadline relative to the resulting next ready time.
        struct ExplicitDelay{std::chrono::nanoseconds delay; std::chrono::nanoseconds rel_deadline;};
        
        std::variant<None, Await, NextCycle, ImplicitDelay, ExplicitDelay> _variant = None{};

    public:
        /// @brief Send no signal to the Task/Scheduler or clear the previous one 
        inline CoTaskSignal& none(){this->_variant = None{}; return *this;}

        /// @brief Send an await signal to the Task/Scheduler. The Coroutine is waiting on an external event (Hardware/IO/other task/etc.) 
        inline CoTaskSignal& await(){this->_variant = Await{}; return *this;}

        /// @brief Send the completion of this cycle and trigger the recalculation of the next one 
        inline CoTaskSignal& next_cycle(){this->_variant = NextCycle{}; return *this;}

        /// @brief Suspend execution and delay the next schedule
        /// @param delay time in ns relative from now
        inline CoTaskSignal& implicit_delay(std::chrono::nanoseconds delay){this->_variant = ImplicitDelay{delay}; return *this;}

        /// @brief Suspend execution and delay the next schedule
        /// @param delay time in ns relative to now
        /// @param rel_deadline time in ns relative to the resulting ready time
        inline CoTaskSignal& explicit_delay(std::chrono::nanoseconds delay, std::chrono::nanoseconds rel_deadline){this->_variant = ExplicitDelay{delay, rel_deadline}; return *this;}

        /// @brief Get the signal type
        /// @returns An enum Type 
        inline Type type() const {return static_cast<Type>(this->_variant.index());}

        /// @brief get the implicit delay time
        /// @returns the implicit delay time
        /// @throws If `ASSERTION_LEVEL_O1` or higher is enabled: throws an AssertionFailure, if the signal does not hold an implicit delay
        inline ImplicitDelay implicit_delay() const {
            EMBED_ASSERT_O1(std::holds_alternative<ImplicitDelay>(this->_variant));
            return std::get<ImplicitDelay>(this->_variant);
        }

        /// @brief get the explicit delay time
        /// @returns the implicit delay time
        /// @throws If `ASSERTION_LEVEL_O1` or higher is enabled: throws an AssertionFailure, if the signal does not hold an explicit delay
        inline ExplicitDelay explicit_delay() const {
            EMBED_ASSERT_O1(std::holds_alternative<ExplicitDelay>(this->_variant));
            return std::get<ExplicitDelay>(this->_variant);
        }
        
    };


    OStream& operator << (OStream& stream, embed::CoTaskSignal::Type type);
    
} // namespace embed


    