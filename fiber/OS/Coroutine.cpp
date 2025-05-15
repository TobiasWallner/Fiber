
#include <fiber/OS/Coroutine.hpp>
#include <fiber/OStream/OStream.hpp>
#include <fiber/OStream/ansi.hpp>

namespace fiber{
    
    Task::Task(Coroutine<fiber::Exit>&& main, std::string_view task_name) noexcept
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
        FIBER_ASSERT_O1(this->is_resumable());
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

    #ifndef FIBER_DISABLE_EXCEPTIONS
        // Variation using exceptions
        void Task::handle_exception(std::exception_ptr except_ptr) {
            try {
                std::rethrow_exception(except_ptr);
            } catch (const fiber::Exception& e) {
                fiber::cerr << e << fiber::endl;
            } catch (const std::exception& e) {
                fiber::cerr << e.what() << fiber::endl;
            } catch (...) {
                fiber::cerr << "[" << fiber::ansi::bright_red << fiber::ansi::bold << "Unknown exception" << fiber::ansi::reset << "]" << fiber::endl;
            }
            fiber::cerr << "    Unhandled exception inside task: " << this->_task_name << ", id: " << this->_id << fiber::endl;
            fiber::cerr << "    Killing task." << fiber::endl;
            this->kill_chain();
            this->_instant_resume = false;
        }
    #else
        // Variation limiting the use of exceptions
        void Task::handle_exception() {
            this->kill_chain();
            this->_instant_resume = false;
        }
    #endif

    void Task::kill_chain(){
        // kill loop
        while(this->_leaf_coroutine){
            CoroutineNode* next = this->_leaf_coroutine->parent();
            this->_leaf_coroutine->destroy();
            this->_leaf_coroutine = next;
        }
    }

}