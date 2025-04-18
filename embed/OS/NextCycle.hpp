#pragma once

#include <embed/OS/Coroutine.hpp>

namespace embed
{
    
    class NextCycle : public AwaitableNode{
        bool _ready = false;
    public:

        inline bool await_ready() const noexcept override {return this->_ready;}

        inline void await_resume() noexcept {}

    private:
        inline CoTaskSignal await_suspend_signal() noexcept override {
            this->_ready = true;
            return CoTaskSignal().next_cycle();
        }
    };

} // namespace embed
