
#include "Coroutine.hpp"
#include "embed/OStream.hpp"

namespace embed{
    
    CoTask::CoTask(const char* task_name, Coroutine<embed::Exit>&& main) noexcept
            : _task_name(task_name)
            , _main_coroutine(std::move(main))
    {
        this->_leaf_coroutine = this->_main_coroutine.node();
        this->_main_coroutine.Register(this);
    }

    CoTask::CoTask(CoTask&& other) noexcept
        : _task_name(other._task_name)
        , _main_coroutine(std::move(other._main_coroutine))
        , _leaf_coroutine(std::move(other._leaf_coroutine))
        , _leaf_awaitable(other._leaf_awaitable)
        , _instant_resume(other._instant_resume) 
    {
        this->_main_coroutine.Register(this); // re-register
        other._task_name = "";
        other._task_id = -1;
        other._leaf_awaitable = nullptr;
        other._instant_resume = false;
    }

    CoTask& CoTask::operator=(CoTask&& other) noexcept {
        if(this != &other){
            this->_task_name = other._task_name;
            this->_task_id = other._task_id;
            this->_main_coroutine = std::move(other._main_coroutine);
            this->_leaf_coroutine = std::move(other._leaf_coroutine);
            this->_leaf_awaitable = other._leaf_awaitable;
            this->_instant_resume = other._instant_resume;
            this->_main_coroutine.Register(this); // re-register

            other._task_id = -1;
            other._task_name = "";
            other._leaf_awaitable = nullptr;
            other._instant_resume = false;
        }
        return *this;
    }

    bool CoTask::is_resumable() const {
        if(this->_leaf_coroutine){
            if(this->_leaf_awaitable != nullptr){
                return this->_leaf_awaitable->await_ready();
            }else{
                return true;
            }
        }else{
            return false;
        }
    }

    bool CoTask::resume(){
        if(this->is_resumable()){
            do{
                this->_instant_resume = false;
                this->_leaf_awaitable = nullptr;
                this->_leaf_coroutine->resume();
            }while(this->_instant_resume); // instantly resume. E.g.: if a new coroutine spawned.
            return true;
        }else{
            return false;
        }
    }

    void CoTask::handle_exception(std::exception_ptr except_ptr) {
        try {
            std::rethrow_exception(except_ptr);
        } catch (const embed::Exception& e) {
            embed::cerr << e << embed::endl;
        } catch (const std::exception& e) {
            embed::cerr << e.what() << embed::endl;
        } catch (...) {
            embed::cerr << "[" << embed::ansi::bright_red << embed::ansi::bold << "Unknown exception" << embed::ansi::reset << "]" << embed::endl;
        }
        
        embed::cerr << "    Unhandled exception inside task: " << this->_task_name << ", id: " << this->_task_id << embed::endl;
        embed::cerr << "    Killing task." << embed::endl;
    
        this->kill_chain();
        this->_exit_status = Exit::Failure;
        this->_instant_resume = false;
    }

    void CoTask::kill_chain(){
        // kill loop
        while(this->_leaf_coroutine){
            CoroutineNode* next = this->_leaf_coroutine->parent();
            this->_leaf_coroutine->destroy();
            this->_leaf_coroutine = next;
        }
        this->_life_cycle = TaskLifeCycle::Killed;
    }

}