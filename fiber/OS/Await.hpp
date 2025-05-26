#pragma once

#include <coroutine>

namespace fiber
{
    
    /**
     * \brief Awaitable that waits for a function to return true
     * 
     * Useful if you need to wait for an condition or register value
     * 
     * Example:
     * ```cpp
     * co_await fiber::TrueAwait([]{return ptr->to->register->bit;})
     * ```
     * 
     * \tparam Callable a callable that returns true
     */
    template<class Callable, bool lvalue_reference = true>
    class AwaitCallable{
        const Callable& _callable;
        const bool _expected;
        public:

        AwaitCallable(bool expected, const Callable& callable) 
            : _callable(callable)
            , _expected(expected){}

        /**
         * \brief Returns the value of the callable
         */
        constexpr bool await_ready() const noexcept {return this->_callable() == this->_expected;}

        /**
         * \brief no operation. For coroutine machinery.
         */
        constexpr void await_resume() {}

        /**
         * @brief no operation. suspends always. For coroutine machinery.
         */
        template<class Handle>
        constexpr void await_suspend([[maybe_unused]]const Handle& handle) noexcept{}
    };

    template<class Callable>
    class AwaitCallable<Callable, false>{
        const Callable _callable;
        const bool _expected;
        public:

        AwaitCallable(bool expected, Callable&& callable) 
            : _callable(std::move(callable))
            , _expected(expected){}

        /**
         * \brief Returns the value of the callable
         */
        constexpr bool await_ready() const noexcept {return this->_callable() == this->_expected;}

        /**
         * \brief no operation. For coroutine machinery.
         */
        constexpr void await_resume() {}

        /**
         * @brief no operation. suspends always. For coroutine machinery.
         */
        template<class Handle>
        constexpr void await_suspend([[maybe_unused]]const Handle& handle) noexcept{}
    };

    template<class Callable>
    AwaitCallable<Callable, true> Await(bool expected, const Callable& callable) {
        return AwaitCallable<Callable, true>(expected, callable);
    }

    template<class Callable>
    AwaitCallable<Callable, false> Await(bool expected, Callable&& callable) {
        return AwaitCallable<Callable, false>(expected, std::move(callable));
    }

    template<class Callable>
    auto AwaitTrue(Callable&& callable) {
        return Await(true, std::forward<Callable>(callable));
    }

    template<class Callable>
    auto AwaitFalse(Callable&& callable) {
        return Await(false, std::forward<Callable>(callable));
    }

    template<class Object, class Method, bool lvalue_reference = true>
    class AwaitMethod{
        const Object& _obj;
        const Method _method;
        const bool _expected;
    public:

        AwaitTrueMethod(bool expected, const Object& obj, Method method) 
            : _obj(obj)
            , _method(method)
            , _expected(expected)
            {}

        /**
         * \brief Returns the value of the callable
         */
        constexpr bool await_ready() const noexcept {
            return this->_obj._method() == this->_expected;
        }

        /**
         * \brief no operation. For coroutine machinery.
         */
        constexpr void await_resume() {}

        /**
         * @brief no operation. suspends always. For coroutine machinery.
         */
        template<class Handle>
        constexpr void await_suspend([[maybe_unused]]const Handle& handle) noexcept{}
    };

    template<class Object, class Method>
    class AwaitMethod<Object, Method, false>{
        const Object _obj;
        const Method _method;
        const bool _expected;
    public:

        AwaitTrueMethod(bool expected, Object&& obj, Method method) 
            : _obj(std::move(obj))
            , _method(method)
            , _expected(expected)
            {}

        /**
         * \brief Returns the value of the callable
         */
        constexpr bool await_ready() const noexcept {
            return this->_obj._method() == this->_expected;
        }

        /**
         * \brief no operation. For coroutine machinery.
         */
        constexpr void await_resume() {}

        /**
         * @brief no operation. suspends always. For coroutine machinery.
         */
        template<class Handle>
        constexpr void await_suspend([[maybe_unused]]const Handle& handle) noexcept{}
    };

    template<class Object, class Method>
    AwaitMethod<Object, Method, true> Await(bool expected, const Object& object, Method method) {
        return AwaitMethod<Object, Method, true>(expected, object, method);
    }

    template<class Object, class Method>
    AwaitMethod<Object, Method, true> Await(bool expected, Object&& object, Method method) {
        return AwaitMethod<Object, Method, true>(expected, std::move(object), method);
    }

    template<class Object, class Method>
    auto AwaitTrue(Object&& object, Method method) {
        return Await(true, std::forward<Object>(object), method);
    }

    template<class Object, class Method>
    auto AwaitFalse(Object&& object, Method method) {
        return Await(false, std::forward<Object>(object), method);
    }

} // namespace fiber
