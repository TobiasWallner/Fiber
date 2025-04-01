#pragma once

//std
#include <coroutine>
#include <chrono>
#include <vector>
#include <string>

//embed
#include "Exit.hpp"
#include "TaskFuture.hpp"

/* 
    I know one should not provide namespaces, but it is just tedious for the suffixes.
    So I just provide it here and reduce that friction. It is just for numbers: 100us - come on.
*/
using namespace std::chrono_literals;





namespace embed{

    enum class TaskLifeCycle{
        New,            ///< Newly created task --> empty coroutine
        Created,        ///< New task with created co-routine
        Waiting,        ///< waiting to become ready. is in the passive running queue. For tasks that are not in avtive window and wait for their next start time.
        Ready,          ///< ready to be executed. is in the active running queue. For tasks which are in the active window [start_time, deadline).
        Running,        ///< current executed task
        Idle,           ///< task that made no progress on the last yield
        ReReady,        ///< task that is ready again
        Disabled,       ///< is in the disabled queue. OS will not touch it until enabled by some external stimulus
        ExitSuccess,    ///< has successfully finished execution - will not be executed again - prepare removal
        ExitFailure,    ///< some error occured that lead to the exit of this task
        Died,           ///< Task died on its own - peacfully - we will send flowers to its parent
        Killed,         ///< someone else killed the task - we will send a special agent to inform the parent
    };

    // foreward declaration
    class OS;
    class BaseTaskTestSuit;

    /** @brief Interface/Base type for a task 
     * 
     * Example:
     * ```cpp
     * 
     * ```
     * 
     * */ 
    class BaseTask{
    public:

        friend class embed::OS;
        friend class embed::BaseTaskTestSuit;
    public:

        /// @brief default task creates the new task on instantiation
        BaseTask(){this->create();}

        /// @brief destructor
        ~BaseTask(){}

        /// @brief Function defined by the user that initialises the first start_time and deadline
        /// @param now The current system time
        virtual void init(std::chrono::nanoseconds now) = 0;

        /// @brief User defined co-routine. The task function that will be run.
        /// @param args A list of optional string arguments that can be passed to the tasks main function
        /// @return a ETaskExit, like `::Success`, or `::Failure`. uses a TaskFuture which delays that return, allowing the task to be suspended and resumed multiple times before the final exit value returns.
        virtual TaskFuture<embed::Exit> main() = 0;

        /// @brief User defined function that updates the schedule for the next time period. Will be executed it the task co_awaits with an `End`.
        virtual void update_schedule() = 0;


        inline bool is_awaiting() const {return this->coroutine.promise().is_awaiting();}

        inline void start_time(const std::chrono::nanoseconds& time) {this->_start_time = time;}
        [[nodiscard]] inline std::chrono::nanoseconds start_time() const {return this->_start_time;}

        inline void deadline(const std::chrono::nanoseconds& time) {this->_deadline = time;}
        [[nodiscard]] inline std::chrono::nanoseconds deadline() const {return this->_deadline;}

    private:
        inline void execution_start(const std::chrono::nanoseconds& time){this->_execution_start = time;}
    public:
        inline std::chrono::nanoseconds execution_start() const {return this->_execution_start;}

    private:
        inline void execution_end(const std::chrono::nanoseconds& time){this->_execution_end = time;}
    public:
        inline std::chrono::nanoseconds execution_end() const {return this->_execution_end;}


        [[nodiscard]] inline TaskLifeCycle life_cycle() const {return this->_state;}
    private:
        inline void life_cycle(TaskLifeCycle new_state) {this->_state = new_state;}
    public:

        inline bool has_been_created() const {return static_cast<bool>(this->coroutine);}
        inline bool is_done() const {return this->coroutine.done();}
        inline bool is_yielding() const {return this->coroutine.promise().is_yielding();}
    private:

        inline void create(){
            this->coroutine = this->main();
            this->life_cycle(TaskLifeCycle::Created);
        }

        inline void create_if(){
            if(!this->has_been_created()){
                this->create();
            }
        }

        inline Exit exit_value() const {
            return this->coroutine.promise().get_return_result();
        }

        inline Suspend suspend_value() const {
            return this->coroutine.promise().get_yield_result();
        }

        inline void resume(){
            this->coroutine.resume();
        }

    private:
        std::chrono::nanoseconds _start_time = 0ns; // scheduled start time after which task execution is allowed
        std::chrono::nanoseconds _deadline = 0ns;   // scheduled deadline before which the task execution should have happened
        
        std::chrono::nanoseconds _execution_start = 0ns; // the measurement of the start of execution
        std::chrono::nanoseconds _execution_end = 0ns;   // the measurement of the end of execution
    
        TaskFuture<embed::Exit> coroutine;
    
        TaskLifeCycle _state = TaskLifeCycle::New;
    };

    
    struct less_start_time{
        bool operator()( const BaseTask* lhs, const BaseTask* rhs ) const{
            return lhs->start_time() < rhs->start_time();
        }
    };

    struct greater_start_time{
        bool operator()( const BaseTask* lhs, const BaseTask* rhs ) const{
            return lhs->start_time() > rhs->start_time();
        }
    };

    struct less_deadline{
        bool operator()( const BaseTask* lhs, const BaseTask* rhs ) const{
            return lhs->start_time() < rhs->start_time();
        }
    };

    struct greater_deadline{
        bool operator()( const BaseTask* lhs, const BaseTask* rhs ) const{
            return lhs->start_time() > rhs->start_time();
        }
    };
    

}//embed

