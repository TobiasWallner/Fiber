#include "OS.hpp"

//std
#include <chrono>
#include <vector>
#include <coroutine>
#include <queue>

//embed
#include "BaseTask.hpp"

    
embed::OS::OS(std::chrono::nanoseconds (*time)(void)) 
    : _time(time)
    , _prev_time(time()){}

std::chrono::nanoseconds embed::OS::time() const {return this->_time();}

void embed::OS::add_task(BaseTask* task){
    task->create_if();
    this->_waiting_queue.push(task);
}

void embed::OS::spin(){
    this->spin_waiting();
    this->spin_suspend_bench();
    this->spin_ready_queue();
}

void embed::OS::spin_ready_queue(){
    BaseTask* task = this->_ready_queue.top();
    this->_ready_queue.pop();

    if(task->life_cycle() == TaskLifeCycle::Ready){
        // if it is ready the first time
        task->execution_start(this->time());
    }

    // run task
    if(!task->is_done()){
        task->life_cycle(TaskLifeCycle::Running);
        task->resume();
    }

    // check return values from `co_return` or `co_yield`
    /* TODO: Re-DO
    if(task->is_done()){
        switch(task->return_value()){
            case Exit::Success : {
                task->life_cycle(TaskLifeCycle::ExitSuccess);
                // do not insert the task back into a queue
                // TODO: inform parent somehow?
                // let the task die
                task->life_cycle(TaskLifeCycle::Died);
            }break;
            case Exit::Failure :
            default : {
                task->life_cycle(TaskLifeCycle::ExitFailure);
                // do not insert the task back into a queue
                // TODO: inform parent somehow?
                // let the task die
                task->life_cycle(TaskLifeCycle::Died);
            }break;
        }
    }else if(task->is_yielding()){
        // from `co_yield` --> check the suspend commands
        switch(task->suspend_value()){
            case SuspendType::Yield : {
                // put task on the suspend bank
                task->life_cycle(TaskLifeCycle::Idle);
                this->_suspend_bench.push_back(task);
            }
            case SuspendType::Cycle : {
                // end this cycle, update the tasks schedule and put the task back on the waiting queue
                task->life_cycle(TaskLifeCycle::Waiting);
                task->execution_end(this->time());
                task->update_schedule();
                this->_waiting_queue.push(task);
            }
            default : {
                task->life_cycle(TaskLifeCycle::ExitFailure);
                // do not insert the task back into a queue
                // TODO: inform parent somehow?
                // let the task die
                task->life_cycle(TaskLifeCycle::Died);
            }break;
        }
    }else{
        // from `co_await`
        // put task on the suspend bank
        task->life_cycle(TaskLifeCycle::Idle);
        this->_suspend_bench.push_back(task);
    }
    */
}

/// @brief puts all tasks that are not idle any more back into the ready list
void embed::OS::spin_suspend_bench(){
    bool result = false;
    auto itr = this->_suspend_bench.begin();
    while(itr != this->_suspend_bench.end()){
        BaseTask* task = *itr;
        if(!task->is_awaiting()){
            this->_suspend_bench.erase(itr);
            task->life_cycle(TaskLifeCycle::ReReady);
            this->_ready_queue.push(task);
        }else{
            ++itr;
        }
    }
}

/// @brief puts all tasks that are past their start time into the ready queue
void embed::OS::spin_waiting(){        
    std::chrono::nanoseconds now = this->time();
    while(this->_waiting_queue.top()->start_time() <= now){
        BaseTask* task = _waiting_queue.top();
        this->_waiting_queue.pop();
        task->life_cycle(TaskLifeCycle::Ready);
        this->_ready_queue.push(task);
    }
    this->_prev_time = now;
}


