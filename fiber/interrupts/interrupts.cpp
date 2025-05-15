#include "fiber/interrupts/interrupts.hpp"
#include "fiber/Exceptions/Exceptions.hpp"

#ifndef FIBER_SINGLE_CORE
    #include <atomic>
#endif

#include <limits>


/*
#if defined(__ARM_ARCH) || defined(__arm__) || defined(__ARM_ARCH_ISA_THUMB) // ARM

    #if defined(__GNUC__)
        #include <cmsis_gcc.h>
    #elif defined(__clang__) 
        #include <cmsis_armclang.h>
    #elif defined(__ICCARM__) 
        #include <cmsis_iccarm.h>
    #endif

    #define ENABLE_INTERRUPTS   __enable_irq()
    #define DISABLE_INTERRUPTS  __disable_irq()

#elif defined(__AVR__) // AVR (GCC or Clang)

    #include <avr/io.h>
    #include <avr/interrupt.h>

    #define ENABLE_INTERRUPTS   sei()
    #define DISABLE_INTERRUPTS  cli()

#elif defined(__ICCAVR__) // AVR (IAR)

    #include <ioavr.h>
    #include <intrinsics.h>

    #define ENABLE_INTERRUPTS   __enable_interrupt()
    #define DISABLE_INTERRUPTS  __disable_interrupt()

#elif defined(__XC8) // PIC 8-bit (XC8)

    #include <xc.h>

    #define ENABLE_INTERRUPTS   ei()
    #define DISABLE_INTERRUPTS  di()

#elif defined(__XC16) || defined(__XC32) // PIC 16/32-bit

    #include <xc.h>

    #define ENABLE_INTERRUPTS   __builtin_enable_interrupts()
    #define DISABLE_INTERRUPTS  __builtin_disable_interrupts()

#elif defined(__riscv) // RISC-V

    #define ENABLE_INTERRUPTS   __asm__ volatile("csrsi mstatus, 0x8")
    #define DISABLE_INTERRUPTS  __asm__ volatile("csrci mstatus, 0x8")

#elif defined(__MSP430__) // MSP430

    #if defined(__GNUC__)
        #include <msp430.h>
        #define ENABLE_INTERRUPTS   __bis_SR_register(GIE)
        #define DISABLE_INTERRUPTS  __bic_SR_register(GIE)
    #else
        #define ENABLE_INTERRUPTS   __enable_interrupt()
        #define DISABLE_INTERRUPTS  __disable_interrupt()
    #endif

#elif defined(ESP_PLATFORM) || defined(__XTENSA__) // ESP32 / ESP8266

    #include "freertos/portmacro.h" // part of ESP-IDF

    #define ENABLE_INTERRUPTS   portENABLE_INTERRUPTS()
    #define DISABLE_INTERRUPTS  portDISABLE_INTERRUPTS()

#elif defined(__C51__) || defined(__SDCC) // 8051 / MCS-51

    // Enable All
    #define ENABLE_INTERRUPTS   (EA = 1)
    #define DISABLE_INTERRUPTS  (EA = 0)

#elif defined(__RX__) || defined(__RENESAS__) // Renesas RX/RL78/RA

    #define ENABLE_INTERRUPTS   __builtin_rx_setpsw_i()   // RX: enable global interrupts
    #define DISABLE_INTERRUPTS  __builtin_rx_clrpsw_i()

#else

    // Default no-ops
    #define ENABLE_INTERRUPTS   ((void)0)
    #define DISABLE_INTERRUPTS  ((void)0)

#endif
*/


namespace fiber{

    #if defined(FIBER_SINGLE_CORE)
        static volatile uint32_t _irq_disable_count = 0;
    #else
        static std::atomic<uint32_t> _irq_disable_count{0};    
    #endif

    FIBER_WEAK void disable_interrupts_command(){
        // empty
    }

    
    FIBER_WEAK void enable_interrupts_command(){
        // empty
    }

    void disable_interrupts(){
        FIBER_ASSERT_CRITICAL(_irq_disable_count != std::numeric_limits<uint32_t>::max());
        #if defined(FIBER_SINGLE_CORE)
            if (_irq_disable_count++ == 0) {
                disable_interrupts_command();
            }
        #else
        if (_irq_disable_count.fetch_add(1, std::memory_order_acq_rel) == 0) {
            disable_interrupts_command();
        }
        #endif

        
    }

    void enable_interrupts(){
        FIBER_ASSERT_CRITICAL(_irq_disable_count != 0);
        #if defined(FIBER_SINGLE_CORE)
            if (_irq_disable_count-- == 1) {
                enable_interrupts_command();
            }
        #else
            if (_irq_disable_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                enable_interrupts_command();
            }
        #endif
    }

} // namespace fiber