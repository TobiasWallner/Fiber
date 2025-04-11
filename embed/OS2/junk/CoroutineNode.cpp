#include "CoroutineNode.hpp"
#include "CoTask.hpp"

namespace embed
{
    class CoroutineNode{
    private:
        CoTask* master = nullptr;
        CoroutineNode* parent = nullptr;

    public: 
        virtual ~CoroutineNode() noexcept = default;

        /*
            Coroutine expansion of `co_await`:
            ```
            auto&& awaitable = expr;
            if (!awaitable.await_ready()) {
                awaitable.await_suspend(current_coroutine_handle);
                co_return;
            }
            auto result = awaitable.await_resume();
            ```
        */

        inline void Register(CoTask* master){
            embed::cout << "CoroutineNode::Register(CoTask*)" << embed::endl;
            this->master = master;
        }

        virtual bool await_ready() const noexcept = 0;

        virtual std::coroutine_handle<> coro_handle() = 0;

        template<class ReturnType>
        inline void await_suspend(std::coroutine_handle<CoroutinePromise<ReturnType>> handle) noexcept {
            embed::cout << "CoroutineNode::await_suspend()" << embed::endl;
            // store parent and master
            this->parent = h.promise().future();
            this->master = parent.master;

            // register leafe in master to tell it what coroutine to resume next
            this->master->register_leaf(coro_handle());
        }

        inline std::suspend_always final_suspend() noexcept {
            embed::cout << "CoroutineNode::final_suspend()" << embed::endl;
            // unregister self and re-register the parent, so next time the master will resume the parent
            this->master->register_leaf(this->parent->coro_handle());
            return {};
        }
    };
} // namespace embed
