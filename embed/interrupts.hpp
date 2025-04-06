#pragma once

// source: interrupts.cpp

namespace embed{

    /**
     * \brief Function pointer that is used to disable the interrupts
     * 
     * Overload this function for your platform to guarantee interrupt safety
     */
    extern void (*disable_interrupts)();

    /**
     * \brief Function pointer that is used to enable the interrupts
     * 
     * Overload this function for your platform to guarantee interrupt safety
     */
    extern void (*enable_interrupts)();


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