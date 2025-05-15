#pragma once

#include <fiber/Core/definitions.hpp>

namespace fiber{

    /**
     * @brief Weak, user overloadable function that provides the command to disable interrupts.
     */
    FIBER_WEAK void disable_interrupts_command();

    /**
     * @brief Weak, user overloadable function that probides the command to enable interrupts.
     */
    FIBER_WEAK void enable_interrupts_command();

    /**
     * \brief Function that is used to disable the interrupts 
     * 
     * This function disables interrupts in a thread safe manner.
     * It will make sure that in a series of calls to `disable_interrupts()`
     * and `enable_interrupts()` only the first will disable it and the 
     * last will enable it. 
     * 
     * It will internally call the user overloadable weak function: 
     * ```cpp
     * void disable_interrupts_command()
     * ```
     * 
     * If you are working on a single core machine without peremption, 
     * consider defining `FIBER_SINGLE_CORE` for a simplified and faster
     * version.
     */
    void disable_interrupts();

    /**
     * \brief Function pointer that is used to enable the interrupts
     * 
     * This function enables interrupts in a thread safe manner.
     * It will make sure that in a series of calls to `disable_interrupts()`
     * and `enable_interrupts()` only the first will disable it and the 
     * last will enable it. 
     * 
     * It will internally call the user overloadable weak function: 
     * ```cpp
     * void enable_interrupts_command()
     * ```
     * 
     * If you are working on a single core machine without peremption, 
     * consider defining `FIBER_SINGLE_CORE` for a simplified and faster
     * version.
     */
    void enable_interrupts();


    /**
     * \brief Makes sure that equaly many interrupt disable and enable calls are beingmade.
     * 
     * Disables the interrupts when being constructed and enables them again when it goes out
     * of scope and is being deconstructed.
     */
    class InterruptGuard{
    public:
        inline InterruptGuard(){disable_interrupts();}
        inline ~InterruptGuard(){enable_interrupts();}
    };

}