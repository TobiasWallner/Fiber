
// std
#include <utility>

// fiber
#include <fiber/OS/Coroutine.hpp>
#include <fiber/OStream/OStream.hpp>
#include <fiber/OStream/ansi.hpp>

namespace fiber{
    
    bool TaskBase::is_resumable() const {
        if(this->_leaf_coroutine && !this->is_done()){
            if(this->_leaf_awaitable_obj != nullptr){
                return this->_leaf_awaitable_ready_func(this->_leaf_awaitable_obj);
            }else{
                return true;
            }
        }
        return false;
    }

    void TaskBase::resume(){
        FIBER_ASSERT_O1(this->is_resumable());
        do{
            this->_instant_resume = false;
            this->_leaf_awaitable_obj = nullptr;
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
        void TaskBase::handle_exception(std::exception_ptr except_ptr) {
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
            this->destroy();
            this->_instant_resume = false;
        }
    #else
        // Variation limiting the use of exceptions
        void TaskBase::handle_exception() {
            this->destroy();
            this->_instant_resume = false;
        }
    #endif

}