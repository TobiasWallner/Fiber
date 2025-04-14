#pragma once

#include <embed/OS/Future.hpp>

namespace embed{

    /**
     * \brief Copies count many bytes from `source` to `dest` in a blocking behaviour.
     * 
     * Internal/default implementation for embed::memcpy that the embed library uses to copy data.
     * Redirect the `embed::memcpy` and `embed::async_memcpy` function pinters to overload it with your custom copy implementation,
     * that for example uses a direct memory access controller (DMA).
     * 
     * \param dest Pointer to the destination to which `count` many bytes will be written
     * \param source Pointer to the source from which `count` many bytes will be read
     * \param count The number of bytes that will be copied
     */
    void _memcpy(void* dest, const void* source, size_t count);

    /**
     * \brief Copies count many bytes from `source` to `dest` in a blocking behaviour.
     * 
     * Internal/default implementation for embed::async_memcpy that the embed library uses to copy data.
     * Redirect the `embed::memcpy` and `embed::async_memcpy` function pinters to overload it with your custom copy implementation,
     * that for example uses a direct memory access controller (DMA).
     * 
     * \param dest Pointer to the destination to which `count` many bytes will be written
     * \param source Pointer to the source from which `count` many bytes will be read
     * \param count The number of bytes that will be copied
     * \param promise The caller will generate a Future that stores a future value. From which this promise is derived.
     *      The Promise will signal to the future when the asynchrunous operation completed.
     */
    void _async_memcpy(void* dest, const void* source, size_t count, Promise<int> promise);

    /**
     * \brief Function that the embed:: library uses to copy memory
     * 
     * The expected behaviour of memcpy is to copy count many bytes from source to dest in a blocking behaviour,
     * aka. the function returns once the copy has completed.
     * 
     * This function pointer can be overloaded by the user for a concrete implementation that uses hardware acceleration.
     * 
     * \param dest Pointer to the destination to which `count` many bytes will be written
     * \param source Pointer to the source from which `count` many bytes will be read
     * \param count The number of bytes that will be copied
     */
    extern void(*memcpy)(void* dest, const void* source, size_t count);

    /**
     * \brief copies from source to dest so that no more bytes than destSize or sourceSize are written
     * \param dest a pointer to the destination that data should be written to
     * \param destSize The number of bytes that can be written to dest
     * \param source a pointer to the source that data should be read from
     * \param sourceSize The number of bytes that can be read from the source
     */
    inline void memcpy_s(void* dest, size_t destSize, const void* source, size_t sourceSize){
        memcpy(dest, source, (destSize < sourceSize) ? destSize : sourceSize);
    }

    /**
     * \brief Copies data from source to dest, without leaving the closed open range [first, last)
     * \param destFirst Pointer to the first byte that should be written to
     * \param destLast Pointer past the last byte that should be written to
     * \param sourceFirst Pointer to the first byte that should be read from
     * \param sourceLast Pointer past the last byte that should be read from
     */
    inline void memcpy_s(void* destFirst, void* destLast, const void* sourceFirst, const void* sourceLast){
        const size_t destSize = reinterpret_cast<const char*>(destLast) - reinterpret_cast<const char*>(destFirst);
        const size_t sourceSize = reinterpret_cast<const char*>(sourceLast) - reinterpret_cast<const char*>(sourceFirst);
        memcpy(destFirst, sourceFirst, (destSize < sourceSize) ? destSize : sourceSize);
    }

    /**
     * \brief Function that the embed library uses to copy memory
     * 
     * The expected behaviour of memcpy is to copy count many bytes from source to dest in a non-blocking behaviour,
     * aka. the function returns immediately after it has beed called which might happen before the copy finished.
     * 
     * This function pointer can be overloaded by the user for a concrete implementation that uses hardware acceleration, 
     * like f.e. a DMA controller that can copy data independently of the main prozessing thread.
     * 
     * Note: the default implementation is a blocking one.
     * 
     * \param dest Pointer to the destination to which `count` many bytes will be written
     * \param source Pointer to the source from which `count` many bytes will be read
     * \param count The number of bytes that will be copied
     * \param promise A promise of a future result derived from a Future type. So: pass a Future in here.
     */
    extern void(*async_memcpy)(void* dest, const void* source, size_t count, Promise<int> promise);

    inline void async_memcpy_s(void* dest, size_t destSize, const void* source, size_t sourceSize, Promise<int> promise){
        async_memcpy(dest, source, (destSize < sourceSize) ? destSize : sourceSize, std::move(promise));
    }

    inline void async_memcpy_s(void* destFirst, void* destLast, const void* sourceFirst, const void* sourceLast, Promise<int> promise){
        const size_t destSize = reinterpret_cast<const char*>(destLast) - reinterpret_cast<const char*>(destFirst);
        const size_t sourceSize = reinterpret_cast<const char*>(sourceLast) - reinterpret_cast<const char*>(sourceFirst);
        const size_t count = (destSize < sourceSize) ? destSize : sourceSize;
        async_memcpy(destFirst, sourceFirst, count, std::move(promise));
    }

}// namespace embed
