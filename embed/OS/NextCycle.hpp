#pragma once

#include <embed/OS/Coroutine.hpp>

namespace embed
{
    
    class NextCycle : public AwaitableNode{
        bool _ready = false;
    public:

        inline bool await_ready() const noexcept final {return this->_ready;}

        inline void await_resume() noexcept {}

    private:
        inline CoSignal await_suspend_signal() noexcept final {
            this->_ready = true;
            return CoSignal().next_cycle();
        }
    };

} // namespace embed
