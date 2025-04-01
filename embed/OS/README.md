# embedOS

**"I won't protect you from yourself, I empower you to do it right."**

A zero-overhead, coroutine-based real-time OS for embedded systems. No interrupts. No preemption. Just a scheduler that does exactly what you tell it to—nothing more, nothing less.

---

## Motivation

Most RTOSes are too heavy, too opinionated, or too unreliable for soft real-time embedded applications. You want deterministic behavior, minimal overhead, and full control. `embedOS` is designed for embedded professionals who know what they're doing and want the OS to stay out of the way.

This project was born from the frustration with existing solutions like FreeRTOS—where stack switching, priority inversion, ISR overhead and unpredictability are the norm. `embedOS` rethinks the problem from first principles.

---

## Who is `embedOS` for?

- Embedded engineers who care about bare-metal.
- Control engineers with strict timing requirements.
- Developers who want modern comfortable software design with almost zero overhead

---

## Design Philosophy

- **No Context Switches**: Tasks are C++20 coroutines that the compiler will turn into switch statements.
- **Cooperative**: Users must explicitly `co_await` or `co_return`. No task is ever preempted.
- **Explicit deadlines**: Finally a real-RTOS. You control timing windows explicitly. Start time and deadline are part of every task.
- **Safe Exceptions**: Uses exceptions safely, with zero heap allocations or overhead. It just jumps from the `throw` directly to your `catch`. No need to pass error values around. Exceptions might even reduce your binary size.
- **Minimal**: Small binary. No interrupts, no stacks, no ISRs. Minimal Flash footprint.
- **Portable**: Just provide a `time()` function and run `.spin()` in your main loop. Fully platform-agnostic, does not depend on any MCU architectures. 
- **Customizeable**: Allows you to overwrite/redirect critical functions like `embed::memcpy()` or default output character streams `embed::cout()`. So you can hook in your own functions that use your on chips DMA controler or USART peripherals.

---

## Example: Basic Periodic Task

```cpp
#include <embed/OS.hpp>

class MyTask : public embed::PeriodicTask {
public:
    task_future<Exit> main() override {
        while (true) {
            co_yield this->yield();     // voluntarily yield to other tasks
            co_await data();            // cooperatively yields when waiting on external events
            co_return Exit::Success;    // end the task
        }
    }
};
```

⚙️ Compile-time control. Runtime zero.  
Just define your allocator and buffer size—embedOS will rewrite and optimize itself entirely at compile time - No setup code, no virtual dispatch. Just pure, structured execution with zero waste.

```cpp

// embedOS uses coroutine frames instead of traditional stack frames.
//      * coroutine frames: only store variables needed to resume operation.
//      * stack frames: store all used and used variables + call stack + register values.
// coroutine frames are thus 5x to 10x smaller.
// Define an allocator that will be used to allocate those frames.
// For a rough estimate use 128 Bytes per Task.
template<typename T>
using MyAlloc = embed::StaticAllocator<T, 1024>;

int main() {
    // redirect the standard output stream to your e.g. USART
    embed::cout = my_usart_output_stream;
    
    // safe and fast try-catch that does not heap allocate
    try{
        // Instantiate at least one OS
        embed::OS<MyAlloc> os(system_time);
        
        // create and add a task
        MyTask task;
        os.add_task(&task);

        // spin that thing
        while (true) {
            os.spin();          
        }
    }catch(std::exception& e){
        // Catch an safe, fast and small exception (no heap allocations happening)
        // Errors regard only programming errors. 
        // Stuff that the programmer did wrong like: allocated too little memory for the coroutine frames.
        embed::cout << e.what();
    }
    
}
```

Your OS is no longer a black box—it's your logic, compiled.

---

## Benefits Over Traditional RTOSes

### Coroutine Frame vs Stack
|              | `embedOS`                  | FreeRTOS                 |
|--------------|----------------------------|--------------------------|
| Per-task mem | 64B-256B frame             | 1kB-4kB stack            |
| Switch type  | Resume/switch statement    | Full context switch      |
| Safety       | Manual control             | OS interrupts any point  |

### Context Switching
|             | `embedOS`                  | FreeRTOS                 |
|-------------|----------------------------|--------------------------|
| Method      | `co_await` / `resume()`    | Interrupt, ISR, stack    |
| Overhead    | 5–50 cycles                | 200–400 cycles           |
| Determinism | High                       | Medium                   |

---

## Philosophy Recap

- **No stacks**
- **No preemption**
- **No vendor bloat**
- **Just clean, coroutine-based, portable structure**

> embedOS is not touching your peripherals. It is just like: you have multiple tasks that you lost track of and want to run with set timings in a state machine/coroutine, here you go, just slap that spin function into your main loop.

> It’s not about measuring time better — it’s about taking time seriously.

---

## License
MIT. Freedom matters.

