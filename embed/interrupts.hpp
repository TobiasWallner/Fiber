#pragma once

// source: interrupts.cpp

namespace embed{

    /**
     * \brief Function pointer that is used to disable the interrupts
     * 
     * Default support for ARM (if `__ARM_ARCH` is defined) and AVR (if `AVR` is defined). 
     * Can be redirected to user defined functions on other platforms
     * 
     * Uses reference counting to guarantee that only the first caller disables the interrups and the last caller enables them.
     * This can be optimized by telling the embed library that one is on a single core system 
     * if the following compile definition is defined: `EMBED_SINGLE_CORE`
     * 
     */
    extern void (*disable_interrupts)();

    /**
     * \brief Function pointer that is used to enable the interrupts
     * 
     * Default support for ARM (if `__ARM_ARCH` is defined) and AVR (if `AVR` is defined). 
     * Can be redirected to user defined functions on other platforms
     * 
     * Uses reference counting to guarantee that only the first caller disables the interrups and the last caller enables them.
     * This can be optimized by telling the embed library that one is on a single core system 
     * if the following compile definition is defined: `EMBED_SINGLE_CORE`
     */
    extern void (*enable_interrupts)();

}