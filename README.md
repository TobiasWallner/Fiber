
Fiber
=======

Version: early alpha

[Documentation](https://tobiaswallner.github.io/Fiber/)

[Repository](https://github.com/TobiasWallner/Fiber)

Copyright © Tobias Wallner

[![gcc-build-and-test](https://github.com/TobiasWallner/Fiber/actions/workflows/test_with_gcc.yml/badge.svg)](https://github.com/TobiasWallner/Fiber/actions/workflows/test_with_gcc.yml) [![clang-build-and-test](https://github.com/TobiasWallner/Fiber/actions/workflows/test_with_clang.yml/badge.svg)](https://github.com/TobiasWallner/Fiber/actions/workflows/test_with_clang.yml)

## ⚡ Motivation

A Fiber-like Cooperative Operating System for Embedded Systems

fiber is a lightweight cooperative operating system for embedded applications built on top of modern C++20 coroutines. It provides a structured, predictable, and memory-safe way to manage multiple tasks without preemption, context switching, or real-time OS overhead.

### What does "fiber-like" mean?

In computing, a fiber is a user-space thread that:

- Runs until it yields control voluntarily (i.e., cooperative multitasking),
- Does not require kernel support or stack switching,
- Is typically very lightweight (a few dozen bytes),
- Has full control over when and how to suspend/resume.

fiber follows this model conceptually, but instead of emulating fibers via thread stacks, it uses:

- C++20 coroutines as the execution primitive,
- `co_await` to represent yield points and scheduling logic,
- A task system that tracks and resumes coroutines based on time, signals, or readiness.

---
## Example

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

### ⏱ Real-Time scheduling

With Fiber, you can declare tasks with explicit timing guarantees:
```cpp
PeriodicTask task(/* period */ 1ms, /* deadline */ 100us);
```

The scheduler enforces **deadlines** with priority queues and can track deadline misses precisely and handles timer/clock overflows. Your tasks run when they must—and only when they should.

### 💡 Stackless, deterministic memory usage

One of the biggest hidden costs in traditional RTOSes is **stack sizing**. You guess how big each task’s stack should be. Too small and you get stack overflows and corrupted memory, which may be uncaught and silently corrupts your memory.

In Fiber each coroutine **allocates automatically and exactly** as much memory as needed, in its **initialisation**. If a coroutine cannot be allocated within the tasks memory, it will throw an exception, which - if uncaught - will print an error message and gracefully kills 🔪 the task.

Further, Fiber never stores full stacks, what is needed to recover from suspension points, reducing memory significantly.

## ✨ Features

- ⚙️ **Cooperative Real-Time Scheduling**
  - The heart of fiber: a **cooperative, coroutine-based, real-time scheduler**
  - Uses `co_await`-based tasks with structured parent-child relationships
  - Deadline-driven scheduling with optional yielding, delaying, awaiting
  - Tiny coroutine task frames (~256B)
  - Fast task/coroutine switching
  - Runs everywhere that comptiles C++20
  - Supports task failure propagation, exception handling, and kill-safe teardown
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

## 🧩 Integration

You can integrate the Fiber library in multiple ways depending on your workflow.
The main target to link against is:

- fiber (link PRIVATE)
- Optionally, fiber_sys_stubs (link PUBLIC) for system call stubs

### (Preferred) [CMake](https://cmake.org/) and [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)

- [CMake](https://cmake.org/) is a build tool generator in witch you can specify how C++ project files should be compiled.
- [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) is a package manager - just a CMake script that you download and include - that will download, integrate, build and cache source libraries for you. I also allows to build everything always from source with the same compile options/flags, which makes this perfect for embedded.

Just add the following ot your `CMakeLists.txt`:
```cmake
# include the CPM package manager script
include(CPM.cmake)

# Optional: Enable system call stubs for freestanding/bare-metal
set(FIBER_USE_FIBER_SYS_STUBS ON CACHE BOOL "" FORCE)

# add/downloads the library
CPMAddPackage("gh:TobiasWallner/Fiber#main")

# link fiber to your project
target_link_libraries(my_target PRIVATE fiber)

# optionally: if `FIBER_USE_FIBER_SYS_STUBS ON`. !!! Has to link PUBLIC !!!
target_link_libraries(my_target PUBLIC fiber_sys_stubs)
```

### [CMake](https://cmake.org/) with its command [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html)

FetchContent, similar to CPM, already comes with CMake. So you do not have to download additional files. However, `FetchContent`
does not support source cacheing and download the full libraries into your project every time.

Add to your `CMakeLists.txt`:
```cmake
# optionally: add system wrappers that reduce binary size on embedded options that never exit `main()`
set(FIBER_USE_FIBER_SYS_STUBS ON CACHE BOOL "" FORCE)

# adds/downloads the library
include(FetchContent)
FetchContent_Declare(
  Fiber
  GIT_REPOSITORY https://github.com/TobiasWallner/Fiber.git
  GIT_TAG main
)

# make the library avaliable
FetchContent_MakeAvailable(Fiber)

# link to the emebed library
target_link_libraries(my_target PRIVATE fiber)

# optionally: if `FIBER_USE_FIBER_SYS_STUBS ON`. !!! Has to link PUBLIC !!!
target_link_libraries(my_target PUBLIC fiber_sys_stubs)
```

### [CMake](https://cmake.org/) with its command [`add_subdirectory`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html)

Download `Fiber` into a subdirectory of your project
```bash
git clone https://github.com/TobiasWallner/Fiber.git external/Fiber --depth=1
```

Add to your `CMakeLists.txt`:
```cmake
# optionally: add system wrappers that reduce binary size on embedded options that never exit `main()`
set(FIBER_USE_FIBER_SYS_STUBS ON CACHE BOOL "" FORCE)

# add the library
add_subdirectory(external/Fiber)

# link to the emebed library
target_link_libraries(my_target PRIVATE fiber)

# optionally: if `FIBER_USE_FIBER_SYS_STUBS ON`. !!! Has to link PUBLIC !!!
target_link_libraries(my_target PUBLIC fiber_sys_stubs) # if ON
```

### 🛠 Don't Want to Use CMake?

No worries. If you're using STM32CubeIDE, Atmel Studio, or Keil:

- Add all .cpp files from `fiber/*` to your project.
- Include `/fiber/` to your include path.
- Start using `#include <fiber/xxx.hpp>` and you're good to go.

Please refere to your IDE vendor on how to include libraries.

❗ But seriously, you should try [CMake](https://cmake.org/). It automates libraries for C++.


## 📜 Licensing

fiber is dual-licensed under a Qt-style model:

See [`LICENSE`](https://github.com/TobiasWallner/Fiber/blob/main/LICENSE)

### ✅ Open Source License (MIT)

You may use fiber freely in open source projects under the following conditions:

- Your project must also be open source (any OSI-approved license).
- Any modifications to fiber must be shared publicly under the same license.
- You must retain attribution to the original author.

See [`LICENSE_OPEN_SOURCE`](https://github.com/TobiasWallner/Fiber/blob/main/LICENSE_OPEN_SOURCE)

### 💼 Commercial License (for Closed/Proprietary Projects)

If you're using fiber in a commercial product or closed-source firmware, you must purchase a commercial license.

With a commercial license, you gain:

- Direct developer support (tiered options)
- License-backed usage rights
- Priority handling for bug reports and feature requests

See [`LICENSE_COMMERCIAL`](https://github.com/TobiasWallner/Fiber/blob/main/LICENSE_COMMERCIAL.md)

👉 Contact: tobias.wallner1@gmx.com for license quotes and support plans.
