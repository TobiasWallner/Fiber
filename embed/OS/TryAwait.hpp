#pragma once

// std
#include <concepts>

// embed
#include <embed/Core/concepts.hpp>
#include <embed/OS/Coroutine.hpp>
#include <embed/Exceptions/Exceptions.hpp>

namespace embed
{

    /**
     * @brief Awaits on something that returns an `std::optional` and throws if it does not have a value.
     * 
     * Example:
     * ```cpp
     * Future<int> async_func(); // returns `std::optional<int>` in `co_await`
     * 
     * Coroutine<Exit> main(){
     * 
     * // TryAwait unpacks the int and throws if the optional does not hold a value
     *  int result = co_await TryAwait(async_func()); 
     * }
     * ```
     */
    template<class T, bool is_rvalue_reference = true>
    class _TryAwait{
    private:
        
    embed::Future<T> _awaitable;

    public:
        /** 
         * @brief Hijacks the other awaitable or Future
         */
        _TryAwait(embed::Future<T>&& awaitable) : _awaitable(std::move(awaitable)){}
        
        /**
         * @brief Forwards the ready call to the real awaitable
         */
        inline bool await_ready() const noexcept {
            return this->_awaitable.await_ready();
        }

        /**
         * @brief Unpacks the `std::optional` from the true awaitables `await_resume` 
         * 
         * Either returns the the unpacked value of the true awaitables `await_resume`
         * or throws an `embed::Exception` if that optional does not contain a value.
         * 
         * @throws `embed::Exception` if the awaitables result of type `std::otional` does not have a value.
         * @returns the unpacked value fot the `std::optional` from the real awaitables `await_resume`
         */
        inline T await_resume() {
            std::optional<T> result = this->_awaitable.await_resume();
            if(result.has_value()){
                return result.value();
            }else{
                EMBED_THROW(Exception("TryAwait received empty `std::optional`"));
            }
        }

        /**
         * @brief Forwards the suspend call to the real awaitable
         */
        template<class ReturnType>
        inline void await_suspend(std::coroutine_handle<embed::CoroutinePromise<ReturnType>> handle) noexcept{
            this->_awaitable.await_suspend(handle);
        }
    };


    /**
     * @brief Specialisation of embed::_TryAwait for lvalue-references
     */
    template<class T>
    class _TryAwait<T, false>{
    private:
        
    embed::Future<T>& _awaitable;

    public:
        /** 
         * @brief Hijacks the other awaitable or Future
         */
        _TryAwait(embed::Future<T>& awaitable) : _awaitable(awaitable){}
        
        /**
         * @brief Forwards the ready call to the real awaitable
         */
        inline bool await_ready() const noexcept {
            return this->_awaitable.await_ready();
        }

        /**
         * @brief Unpacks the `std::optional` from the true awaitables `await_resume` 
         * 
         * Either returns the the unpacked value of the true awaitables `await_resume`
         * or throws an `embed::Exception` if that optional does not contain a value.
         * 
         * @throws `embed::Exception` if the awaitables result of type `std::otional` does not have a value.
         * @returns the unpacked value fot the `std::optional` from the real awaitables `await_resume`
         */
        inline T await_resume() {
            std::optional<T> result = this->_awaitable.await_resume();
            if(result.has_value()){
                return result.value();
            }else{
                EMBED_THROW(Exception("TryAwait received empty `std::optional`"));
            }
        }

        /**
         * @brief Forwards the suspend call to the real awaitable
         */
        template<class ReturnType>
        inline auto await_suspend(std::coroutine_handle<embed::CoroutinePromise<ReturnType>> handle) noexcept{
            if constexpr (requires {this->_awaitable.await_suspend(handle);} ){
                return this->_awaitable.await_suspend(handle);
            }else{
                return void();
            }
        }
    };
    

    /**
     * @brief Wraps an awaitable that returns an `std::optional` conditionally unpacks it or throws if there is no value
     * 
     * Example:
     * ```cpp
     * Future<int> async_func(); // returns `std::optional<int>` in `co_await`
     * 
     * Coroutine<Exit> main(){
     * 
     * // TryAwait unpacks the int and throws if the optional does not hold a value
     *  int result = co_await TryAwait(async_func()); 
     * }
     * ```
     */
    template<class T>
    _TryAwait<T, true> TryAwait(embed::Future<T>&& future) {
        return _TryAwait<T, true>(std::move(future));
    }

    /**
     * @brief Wraps an awaitable that returns an `std::optional` conditionally unpacks it or throws if there is no value
     * 
     * Example:
     * ```cpp
     * Future<int> async_func(); // returns `std::optional<int>` in `co_await`
     * 
     * Coroutine<Exit> main(){
     * 
     * // TryAwait unpacks the int and throws if the optional does not hold a value
     *  int result = co_await TryAwait(async_func()); 
     * }
     * ```
     */
    template<class T>
    _TryAwait<T, false> TryAwait(embed::Future<T>& future) {
        return _TryAwait<T, false>(future);
    }


} // namespace embed
