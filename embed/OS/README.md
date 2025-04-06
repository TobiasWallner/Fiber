# embedOS

**"I won't protect you from yourself, I empower you to do it right."**

A zero-overhead, coroutine-based real-time OS for embedded systems. No interrupts. No preemption. Just a scheduler that does exactly what you tell it to—nothing more, nothing less.

---

## Motivation

Most RTOSes are too heavy, too opinionated, or too unreliable for soft real-time embedded applications. You want deterministic behavior, minimal overhead, and full control. `embedOS` is designed for embedded professionals who know what they're doing and want the OS to stay out of the way.

This project was born from the frustration with existing solutions like FreeRTOS—where stack switching, priority inversion, ISR overhead and unpredictability are the norm. `embedOS` rethinks the problem from first principles.

---

## Who is embedOS for?

If you’re unsure whether embedOS is right for your project, this section will help you decide—and if it’s not, we’ll gladly point you toward alternatives that might serve you better. We’re not here to compete with every RTOS—we're here to **fill a niche**. We believe in putting the power back in the engineer’s hands with a minimal and deterministic scheduling model that gets out of your way.

Here’s how embedOS compares to other common choices:

| Feature    | **Zephyr**     | **FreeRTOS**   | **embedOS** (this project) |
|------------|----------------|----------------|----------------------------|
| **Philosophy** | Full-featured ecosystem  | Lightweight priority based task management | **real Real-Time** deadline based task management with high CPU efficiency |
| **Use Case** | IoT, industrial, networking | General embedded apps | **"real" Real-time control**, **constrained systems**, **highly-dependable/predictable systems** |
| **Scheduling** | complex priority-based, preemptive | priority-based, preemptive, time slicing | **deadline-based**, cooperative |
| **Threading Model** | Full threads, context switching, per-thread stacks | Tasks with independent stacks and context switching    | **Coroutines** coroutine frame buffers instead of stacks, no context switching, state machine task management|
| **Memory Usage per Task** | High (>1kB), full-stack, depends on configuration and features | Moderate (>1kB), requires full-stack per task  | **Extremely low** (~128B), no stacks, small coroutine frames|
| **Binary Footprint** | High, 300kB–500kB+ depending on config | Low, ~10kB–100kB | **<10kB for Scheduler**, 30–100kB including extras |
| **Peripheral Handling** | Device tree, HAL, built-in driver APIs | User-defined, often with vendor HAL | **No interference**—you write your own, OS doesn't touch your peripherals |
| **Context Switch Overhead**| ~200–1500 cycles, sometimes more | 100–600 cycles (stack switch + cpu register save/load) | 30–100 CPU cycles (function call + frame jump) |
| **Platform Support** | Pre-built boards + some MCUs and SoCs  | Wide MCU support, especially with vendor integrations | **Widest support**—designed to run anywhere that compiles C++. Not tied to any platform or architecture |
| **Build System** | CMake + Kconfig (Linux-style) | PlatformIO/Make/CMake | CMake/Your own build system. embedOS is just a simple library. Package manager support: `CPM.cmake` |

If you still don’t know which one to pick, reach out—we’ll help you find the right one, even if it’s not us.

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
        // ============= Cycle 1 =============
        // async communication with another controller

        send_data();
        
        data = co_await receive_data(); // suspends task here - switches to another one - resumes if data has been received

        calculation(data);

        co_yield embed::Cycle();     // ends period cycle, continues computation in the next period

        // ============= Cycle 2 =============
        bool finished = false;
        while(!finished){
            data = calculation(data);
            finished = condition(data);
            co_yield embed::Yield();    // manually yield to other tasks
        }

        co_yield embed::Cycle();     // ends period, continues computation in the next period

        // ============= Cycle 2 =============
        // program script like animations

        drive_fwd();                // start driveing forward
        co_yield 200ms;             // yield execution for 200ms

        turn_right();               // start turning right
        co_yield 10ms;              // yield execution for 10ms

        co_return Exit::Success;    // end the task successfully
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
using MyAlloc = embed::StaticAllocator<T, 128 * 10>;

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

---

## Fault-Error-Failure and Exception Model

A critical question often asked is: when are exceptions thrown?

`embed` uses the error propagation model as per definitions  (🔍 per IEC 61508, Avionics DO-178C, and Laprie taxonomy)

| Term | Definition |
| ---- | ---- |
| Fault | The cause of an error—e.g. a bug, hardware issue, or design flaw |
| Error | A deviation from the correct or expected internal state |
| Failure | An external deviation in delivered service—observable by the system's users |

Exceptions are thrown on the propagation from error --> failure.

Example:
| Event | Term | Notes |
| ---- | ---- |
| Heap/arena is too small | Fault | May be a configuration error or bad design assumption |
| returns invalid pointer |	Error |Internal state no longer matches valid memory model |
| embed::AllocationFailure | Failure | is thrown, task crashes or is aborted |

---

## Integration

### (Preferred) [CMake](https://cmake.org/) and [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)

- [CMake](https://cmake.org/) is a build tool generator in witch you can specify how C++ project files should be compiled.
- [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) is a package manager - just a CMake script that you download and include - that will download, integrate, build and cache source libraries for you. I also allows to build everything always from source with the same compile options/flags, which makes this perfect for embedded.

### [CMake](https://cmake.org/) with its command [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html)

### [CMake](https://cmake.org/) with its command [`add_subdirectory`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html)

### 🛠 Don't Want to Use CMake?

No worries. If you're using STM32CubeIDE, Atmel Studio, or Keil:

- Add all .cpp files from embed/src/ to your project.
- Add embed/include/ to your include path.
- Start using #include <embed/xxx.hpp> and you're good to go.

❗ But seriously, you should try CMake. It's awesome.

---

## License
MIT. Freedom matters.

