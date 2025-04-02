#include "embed/interrupts.hpp"
#include "embed/Exceptions.hpp"

#if !defined(EMBED_SINGLE_CORE)
        #include <atomic>
        static std::atomic<uint32_t> _irq_disable_count{0};
    #else
        static volatile uint32_t _irq_disable_count = 0;
    #endif

#if defined(__ARM_ARCH)
    extern void __enable_irq(void);
    extern void __disable_irq(void);
#elif defined(AVR)
    extern void cli(void);
    extern void sei(void);
#endif


namespace embed{

    /**
     * \brief default function that disables interrupts with nesting support
     */
    void default_disable_interrupts() {
        if (++_irq_disable_count == 1) {
            #if defined(__ARM_ARCH)
                __disable_irq();
            #elif defined(AVR)
                cli();
            #else
                #warning "default_disable_interrupts is not defined for this architecture"
            #endif
        }
    }

    /**
     * \brief default function that enables interrupts with nesting support
     */
    void default_enable_interrupts() {
        if (_irq_disable_count == 0) {
            throw Exception(
                "Error: "
                " In function `default_enable_interrupts()`:"
                " unbalanced number of `default_disable_interrupts()` and `default_enable_interrupts()` calls."
                " Interrupts are already enabled");
        }

        if (--_irq_disable_count == 0) {
            #if defined(__ARM_ARCH)
                __enable_irq();
            #elif defined(AVR)
                sei();
            #else
                #warning "default_enable_interrupts is not defined for this architecture"
            #endif
        }
    }


    void (*disable_interrupts)() = default_disable_interrupts;
    void (*enable_interrupts)() = default_enable_interrupts;

} // namespace embed
