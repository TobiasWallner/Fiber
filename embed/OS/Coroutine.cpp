
#include <embed/OS/Coroutine.hpp>
#include <embed/OStream/OStream.hpp>
#include <embed/OStream/ansi.hpp>

namespace embed{
    
    Task::Task(Coroutine<embed::Exit>&& main, std::string_view task_name) noexcept
            : _task_name(task_name)
            , _main_coroutine(std::move(main))
    {
        this->_leaf_coroutine = this->_main_coroutine.node();
        this->_main_coroutine.Register(this);
    }

    Task::Task(Task&& other) noexcept
        : _task_name(other._task_name)
        , _main_coroutine(std::move(other._main_coroutine))
        , _leaf_coroutine(std::move(other._leaf_coroutine))
        , _leaf_awaitable(other._leaf_awaitable)
        , _instant_resume(other._instant_resume) 
    {
        this->_main_coroutine.Register(this); // re-register
        other._task_name = "";
        other._id = -1;
        other._leaf_awaitable = nullptr;
        other._instant_resume = false;
    }

    Task& Task::operator=(Task&& other) noexcept {
        if(this != &other){
            this->_task_name = other._task_name;
            this->_id = other._id;
            this->_main_coroutine = std::move(other._main_coroutine);
            this->_leaf_coroutine = std::move(other._leaf_coroutine);
            this->_leaf_awaitable = other._leaf_awaitable;
            this->_instant_resume = other._instant_resume;
            this->_main_coroutine.Register(this); // re-register

            other._id = -1;
            other._task_name = "";
            other._leaf_awaitable = nullptr;
            other._instant_resume = false;
        }
        return *this;
    }

    

    bool Task::is_resumable() const {
        if(this->_leaf_coroutine && !this->is_done()){
            if(this->_leaf_awaitable != nullptr){
                return this->_leaf_awaitable->await_ready();
            }else{
                return true;
            }
        }
        return false;
    }

    void Task::resume(){
        EMBED_ASSERT_O1(this->is_resumable());
        do{
            this->_instant_resume = false;
            this->_leaf_awaitable = nullptr;
            this->_leaf_coroutine->resume();
        }while(this->_instant_resume); // instantly resume. E.g.: if a new coroutine spawned.
        /*
        `_instant_resume` guarantees temporal ownership: 
            the task keeps the CPU until its logical atomic unit is complete. 
            This is how priority enforcement and execution atomicity are preserved across coroutine boundaries.
        */
    }

    void Task::handle_exception(std::exception_ptr except_ptr) {
        try {
            std::rethrow_exception(except_ptr);
        } catch (const embed::Exception& e) {
            embed::cerr << e << embed::endl;
        } catch (const std::exception& e) {
            embed::cerr << e.what() << embed::endl;
        } catch (...) {
            embed::cerr << "[" << embed::ansi::bright_red << embed::ansi::bold << "Unknown exception" << embed::ansi::reset << "]" << embed::endl;
        }
        
        embed::cerr << "    Unhandled exception inside task: " << this->_task_name << ", id: " << this->_id << embed::endl;
        embed::cerr << "    Killing task." << embed::endl;
    
        this->kill_chain();
        this->_instant_resume = false;
    }

    void Task::kill_chain(){
        // kill loop
        while(this->_leaf_coroutine){
            CoroutineNode* next = this->_leaf_coroutine->parent();
            this->_leaf_coroutine->destroy();
            this->_leaf_coroutine = next;
        }
    }

}