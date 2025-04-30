
EmbedOS
=======

Version: early alpha

[Documentation](https://tobiaswallner.github.io/embedOS/)

Copyright © Tobias Wallner

[![gcc-build-and-test](https://github.com/TobiasWallner/embedOS/actions/workflows/test_with_gcc.yml/badge.svg)](https://github.com/TobiasWallner/embedOS/actions/workflows/test_with_gcc.yml) [![clang-build-and-test](https://github.com/TobiasWallner/embedOS/actions/workflows/test_with_clang.yml/badge.svg)](https://github.com/TobiasWallner/embedOS/actions/workflows/test_with_clang.yml)

## ⚡ Motivation

Modern embedded systems demand more than just correctness—they demand **performance**, **dependability**, **Real-Time** and **predictability**.

Traditional RTOSes rely on **context switching**, are **stack-heavy** and use **interrupt driven preemption**, which introduce runtime overhead and memory waste.

> **What if task switching is just a state machine?**

### 🌀 Coroutine-powered, zero-waste tasking

By using **C++20 coroutines**, tasks in `embedOS` are structured as lightweight state machines with no context-switching and interrupt overhead. The syntax is modern, expressive, and reads like a simple switch, no unreadable branching necessary:

```cpp
Coroutine<Exit> coroutine(){
  send_data(tx_data);

  // Task suspends — another task runs
  auto rx_data = co_await receive_data();

  // Start async HW operation
  Future<uint32_t> future_crc = hardware_crc(rx_data); 

  // Use CPU in the meantime
  value = calculate(rx_data);

  // Yield until HW finishes
  uint32_t crc co_await future_crc;
  co_return Exit::Success;
}
```
Each task suspends explicitly at `co_await` points, allowing others to run. No busy-waiting, no polling, no thread stacks. Just pure, cooperative, non-blocking execution.

The OS is just the scheduler of your choice, add some tasks and let it spin.
```cpp
int main(){
  Task task(coroutine(), "task");

  embed::StaticLinearScheduler scheduler;
  scheduler.addTask(task);

  while(true){
    scheduler.spin();
  }
}
```

Want multithreading? Just create a scheduler for each core. 



### ⏱ Real-Time scheduling

With embedOS, you can declare tasks with explicit timing guarantees:
```cpp
PeriodicTask task(/* period */ 1ms, /* deadline */ 100us);
```

The scheduler enforces **deadlines** with priority queues and can track deadline misses precisely and handles timer/clock overflows. Your tasks run when they must—and only when they should.

### 💡 Stackless, deterministic memory usage

One of the biggest hidden costs in traditional RTOSes is **stack sizing**. You guess how big each task’s stack should be. 
- Too small and you get stack overflows and corrupted memory. 
- Too large and you waste precious RAM.

#### 🎯 In embedOS, there's no guessing.

Each coroutine task **allocates exactly** as much memory as needed, in the **initialisation**, to store its state at suspension points. The system never stores full stacks—only the **delta state**, reducing memory usage by 5x to 10x compared to traditional context switching.

No stack overflow. No corrupted memory. No waste. Just predictable, dependable engineering.

### 🔐 Dependability by Design

`embedOS` is built with dependability as a first-class concern, not a reactive patch.

Unlike traditional RTOSes that rely on threads, shared stacks, and weak exception handling, `embedOS` uses structured coroutines, deterministic control flow, and explicit task ownership to provide built-in fault isolation and graceful failure handling.

Each task is fully isolated: an exception only affects that task. The rest of the system runs uninterrupted.

What This Means for You:
- A single task crashing won’t bring down your system
- You don’t need hardware MMU or MPU protection to isolate behavior
- You get the freedom of concurrency with the safety of separation
- You can log, recover, or restart failed tasks at runtime
- embedOS brings fail-operational behavior to bare-metal systems
- without threads, without stacks, without interrupts
- on a single core/or multi if you feel fancy.


## 🔍 Who is embedOS for?

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

## ✨ Features

- ⚙️ **Cooperative Real-Time Scheduling (`embedOS`)**
  - The heart of embed: a **cooperative, coroutine-based, real-time scheduler**
  - Uses `co_await`-based tasks with structured parent-child relationships
  - Deadline-driven scheduling with optional yielding, delaying, awaiting
  - No threads, no stacks—just pure control flow
  - Supports task failure propagation, exception handling, and kill-safe teardown
- 🧮 **Fixed-Size, Stack-Friendly Containers**
  - `ArrayList`, `PriorityQueue`, `StaticLinearAllocator` and more
  - Designed for bounded memory environments (MCUs, no heap)
  - Fast, safe, and easy to reason about
- 🔧 **Custom Allocator Support**
  - Easily override coroutine frame allocation, exception memory, dynamic data
  - Control exactly how and where memory is used—no malloc required
- 🖨️ **Powerful Output Streaming**
  - Fully featured `OStream` with `operator<<` formatting
  - Supports strings, booleans, integers, floats (scientific/engineering) (small footprint)
  - Width, alignment, padding, decimal/thousand separators
  - Optional ANSI color formatting for terminal output
- 🪓 **System Stub Overrides (Binary Size Killers)**
  - Replaces exit-to-main code, because embedded never exits
  - Removes unused libc glue code
  - Up to **90% binary size reduction** when enabling exceptions on Cortex-M


## 🧠 Design Philosophy

- **No Context Switches**: Tasks are C++20 coroutines that the compiler will turn into switch statements.
- **Cooperative**: Users must explicitly `co_await` or `co_return`. No task is ever preempted.
- **Explicit deadlines**: Finally a real-RTOS. You control timing windows explicitly. Start time and deadline are part of every task.
- **Safe Exceptions**: Uses exceptions safely, with zero heap allocations or overhead. It just jumps from the `throw` directly to your `catch`. No need to pass error values around. Exceptions might even reduce your binary size.
- **Minimal**: Small binary. No interrupts, no stacks, no ISRs. Minimal Flash footprint.
- **Portable**: Just provide a `time()` function and run `.spin()` in your main loop. Fully platform-agnostic, does not depend on any MCU architectures. 
- **Customizeable**: Allows you to overwrite/redirect critical functions like `embed::memcpy()` or default output character streams `embed::cout()`. So you can hook in your own functions that use your on chips DMA controler or USART peripherals.

> "At the crossroads of optimization and correctness, remember: optimization saves runtime — but correctness saves debug time. Choose which time you truly want to optimize."



## 🧩 Integration

You can integrate the embedOS library in multiple ways depending on your workflow.
The main target to link against is:

- embed (link PRIVATE)
- Optionally, embed_sys_stubs (link PUBLIC) for system call stubs

### (Preferred) [CMake](https://cmake.org/) and [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)

- [CMake](https://cmake.org/) is a build tool generator in witch you can specify how C++ project files should be compiled.
- [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) is a package manager - just a CMake script that you download and include - that will download, integrate, build and cache source libraries for you. I also allows to build everything always from source with the same compile options/flags, which makes this perfect for embedded.

Just add the following ot your `CMakeLists.txt`:
```cmake
# include the CPM package manager script
include(CPM.cmake)

# Optional: Enable system call stubs for freestanding/bare-metal
set(EMBED_USE_EMBED_SYS_STUBS ON CACHE BOOL "" FORCE)

# add/downloads the library
CPMAddPackage("gh:TobiasWallner/embedOS#main")

# link embed to your project
target_link_libraries(my_target PRIVATE embed)

# optionally: if `EMBED_USE_EMBED_SYS_STUBS ON`. !!! Has to link PUBLIC !!!
target_link_libraries(my_target PUBLIC embed_sys_stubs)
```

### [CMake](https://cmake.org/) with its command [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html)

FetchContent, similar to CPM, already comes with CMake. So you do not have to download additional files. However, `FetchContent`
does not support source cacheing and download the full libraries into your project every time.

Add to your `CMakeLists.txt`:
```cmake
# optionally: add system wrappers that reduce binary size on embedded options that never exit `main()`
set(EMBED_USE_EMBED_SYS_STUBS ON CACHE BOOL "" FORCE)

# adds/downloads the library
include(FetchContent)
FetchContent_Declare(
  embedOS
  GIT_REPOSITORY https://github.com/TobiasWallner/embedOS.git
  GIT_TAG main
)

# make the library avaliable
FetchContent_MakeAvailable(embedOS)

# link to the emebed library
target_link_libraries(my_target PRIVATE embed)

# optionally: if `EMBED_USE_EMBED_SYS_STUBS ON`. !!! Has to link PUBLIC !!!
target_link_libraries(my_target PUBLIC embed_sys_stubs)
```

### [CMake](https://cmake.org/) with its command [`add_subdirectory`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html)

Download `embedOS` into a subdirectory of your project
```bash
git clone https://github.com/TobiasWallner/embedOS.git external/embedOS --depth=1
```

Add to your `CMakeLists.txt`:
```cmake
# optionally: add system wrappers that reduce binary size on embedded options that never exit `main()`
set(EMBED_USE_EMBED_SYS_STUBS ON CACHE BOOL "" FORCE)

# add the library
add_subdirectory(external/embedOS)

# link to the emebed library
target_link_libraries(my_target PRIVATE embed)

# optionally: if `EMBED_USE_EMBED_SYS_STUBS ON`. !!! Has to link PUBLIC !!!
target_link_libraries(my_target PUBLIC embed_sys_stubs) # if ON
```

### 🛠 Don't Want to Use CMake?

No worries. If you're using STM32CubeIDE, Atmel Studio, or Keil:

- Add all .cpp files from `embed/*` to your project.
- Include `/embed/` to your include path.
- Start using `#include <embed/xxx.hpp>` and you're good to go.

Please refere to your IDE vendor on how to include libraries.

❗ But seriously, you should try [CMake](https://cmake.org/). It automates libraries for C++.


## 📜 Licensing

embed is dual-licensed under a Qt-style model:

See [`LICENSE`](https://github.com/TobiasWallner/embedOS/blob/main/LICENSE)

### ✅ Open Source License (MIT)

You may use embed freely in open source projects under the following conditions:

- Your project must also be open source (any OSI-approved license).
- Any modifications to embed must be shared publicly under the same license.
- You must retain attribution to the original author.

See [`LICENSE_OPEN_SOURCE`](https://github.com/TobiasWallner/embedOS/blob/main/LICENSE_OPEN_SOURCE)

### 💼 Commercial License (for Closed/Proprietary Projects)

If you're using embed in a commercial product or closed-source firmware, you must purchase a commercial license.

With a commercial license, you gain:

- Direct developer support (tiered options)
- License-backed usage rights
- Priority handling for bug reports and feature requests

See [`LICENSE_COMMERCIAL`](https://github.com/TobiasWallner/embedOS/blob/main/LICENSE_COMMERCIAL.md)

👉 Contact: tobias.wallner1@gmx.com for license quotes and support plans.

## Quotes:

> "There comes a point where the objects begin to speak in ways you never planned. If each method was correct, their conversation will be too. That’s when your code becomes alive."  
> - Tobias Wallner

