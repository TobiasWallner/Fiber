#pragma once

#ifndef EMBED_SINGLE_CORE
    #include <atomic>
#endif

#include <coroutine>
#include <tuple>

#include "embed/Exceptions/Exceptions.hpp"
#include "embed/interrupts/interrupts.hpp"

namespace embed{    

    template<class T>
    struct FuturePromisePair;

    template<class T>
    FuturePromisePair<T> make_future_promise();

    template<class T>
    class Promise;

    /**
     * \brief Future and Promise pairs are used to synchronise values between asynchronous tasks.
     * 
     * If you are on a single core bare-metal embedded system (aka. all context switching is interrupt driven)
     * you can set the `EMBED_SINGLE_CORE` definiteion flag for optimizations - smaller binary and faster execution. 
     * It will then remove all the locks and logic that is needed for multi-core thread safety.
     * 
     * Example: one wants to copy data asynchronously (for example with a DMA controller) while doing some computations in the mean time.
     * ```cpp
     * {
     * 
     *      // You want to calculate for example the CRC code of some data and use a hardware module that can do that independently
     *      // The function issues a request to a hardware-unit/co-processor and returns immediately
     *      // create a future that expects a result in the future
     *      Future<int> future_crc = async_crc(data, dataLength);;
     * 
     *      future_crc.is_ready(); // returns `false`
     * 
     *      // In the mean time, while the CRC is being calculated, one can do some calculations
     *      // ...
     *      // ... some math ...
     *      // ...
     *      
     *      // later you you want to get the value from the future - aka. the CRC calculation you call:
     *      int crc = future_crc.get(); // the method waits and blocks this thread until the future value is ready
     * 
     * } // alternatively, if you do not wait or get the value, the future and promise safely detatch in the destructor
     * 
     * ```
     * 
     * To create a future promise pair use:
     * ```cpp
     * auto [future, promise] = embed::make_future_promise<int>();
     * ```
     */
    template<class T>
    class Future{
        private:
            enum class State{
                Busy,   ///< signals that the object is not finished and one has to wait
                HasValue,   ///< signal that the object is finished and can be accessed
                BrokenPromise, ///< signals that the promise was not kept --> throws an exception
            };

            enum class Lock{
                Open,   ///< the lock is open and can be acquired
                Closed, ///< the lock is closed and cannot be acquired
            };
            
            Promise<T>* _promisePtr = nullptr;
            T _value;

            #ifndef EMBED_SINGLE_CORE
                std::atomic<State> _state = State::Busy; // volatile because this will be set by a different threat.
                std::atomic<Lock> _lock = Lock::Open;
            #else
                volatile State _state = State::Busy; // volatile because this will be set by a different threat.
            #endif

            friend class Promise<T>;
        public:
    
            friend FuturePromisePair<T> make_future_promise<T>();

            Future() = default;
            Future(const Future&) = delete;
            Future& operator=(const Future&) = delete;

            Future& operator=(Future&& oldFuture){
                if(this != &oldFuture){
                    this->acquire_dual_locks();
                    switch(oldFuture.get_state()){
                        case State::HasValue: {
                            this->_value = oldFuture._value;
                            this->set_state(State::HasValue);
                        } break;
                        case State::BrokenPromise: {
                            this->set_state(State::BrokenPromise);
                        } break;
                        case State::Busy: {
                            // double check, because after the first check there migth have been an interrupt or async write
                            switch(this->get_state()){
                                case State::HasValue: {
                                    this->_value = oldFuture._value;
                                    this->set_state(State::HasValue);
                                } break;
                                case State::BrokenPromise: {
                                    this->set_state(State::BrokenPromise);
                                } break;
                                case State::Busy: {
                                    // assign new pointer to the promise
                                    if(this->_promisePtr != nullptr){
                                        this->_promisePtr->_futurePtr = this;
                                    }else{
                                        // assign error if there is no linked promise
                                        this->set_state(State::BrokenPromise);
                                    }
                                }
                            }
                        } break;
                    }
                    this->release_dual_locks();
                }
                return *this;
            }
            /// @brief Thread and interrupt save move that re-registers stack pointer in between the future and the promise
            Future(Future&& oldFuture){
                *this = std::move(oldFuture);
            }
            ~Future(){
                // detatch from the promise
                this->acquire_dual_locks();
                this->release_dual_locks_and_detatch();
            }

            /**
             * \brief Blocks the current thread until the value is ready or an error occured.
             */
            void wait() {while(this->is_busy()){/* Do Nothing */}}
    
            /**
             * \brief returns the value of the future and waits if necessary
             * \throws Exception of type std::exception if an error occured.
             */
            T& get(){
                this->wait();
                if(this->is_ready()){
                    return this->_value;
                }else{
                    throw Exception("Read from broken promise in method `Future::get()`.");
                }
            }
    
            /**
             * \brief returns the value of the future and waits if necessary
             * \throws Exception of type std::exception if an error occured.
             */
            [[nodiscard]]inline T* get_if() {
                this->wait();
                if(this->is_ready()){
                    return &(this->_value);
                }else{
                    return nullptr;
                }
            }
    
            [[nodiscard]]inline State get_state() const {
                #ifndef EMBED_SINGLE_CORE
                    return this->_state.load(std::memory_order_acquire);
                #else
                    return this->_state;
                #endif
            }
        
        private:
            inline void set_state(State state) {
                #ifndef EMBED_SINGLE_CORE
                    this->_state.store(state, std::memory_order_release);
                #else
                    this->_state = state;
                #endif
            }
        public:

            /// \brief returns true if the value is ready to read 
            [[nodiscard]]inline bool is_ready() const {return this->get_state() == State::HasValue;}

            /// @brief Provide a callback that will be called if the promise is kept
            /// @details Provideing a callback will redirect the promise to call the callback instead of notifieing the future. 
            /// The future will be invalid and can no longer be used to receive the value. Further, only one callback can be set. 
            /// Calling this function twice will throw an error. A promis must have been created before calling this function.
            /// The state of this future will turned into a 'broken promise'.
            /// @param callback a function pointer that will be called once the Promis receives a value.
            /// @throws a std::exception if this function is called twice or if there is no promis attatched
            void on_ready(void (*callback)(const T&)){
                // if the future is already ready and the promise was kept --> call the callback directly
                if(this->is_ready()){
                    // call the callback
                    callback(this->get());
                    this->set_state(State::BrokenPromise); // for consistency
                }else{
                    // get lock
                    this->acquire_dual_locks();

                    // double check - an interrupt might have fired between the first check and the lock
                    if(this->is_ready()){
                        // release lock in controll path 1/2
                        this->release_dual_locks();

                        // call the callback
                        callback(this->get());
                        this->set_state(State::BrokenPromise); // for consistency
                    }else{
                        // safely install the callback function and detatch the future from the promise
                        if(this->_promisePtr == nullptr){
                            throw Exception("Error: `Future::on_read()` called on detatched Future. A Promise must have created before calling this function.");
                        }else if(this->_promisePtr->_callback != nullptr){
                            throw Exception("Error: `Future::on_read()` called twice on the same Future.");
                        }else{
                            // install callback
                            this->_promisePtr->_callback = callback;
                            this->set_state(State::BrokenPromise); // for consistency
                        }

                        // release lock in controll path 2/2
                        this->release_dual_locks_and_detatch();
                    }
                }
            }

            /// \brief converts to a boolean. same as calling `is_ready()`.
            [[nodiscard]]inline operator bool() const {return this->is_ready();}

            /// @brief returns true if the result is not finished yet and one has to wait
            [[nodiscard]]inline bool is_busy() const {return this->get_state() == State::Busy;}
    
            /**
             * \brief return true if the promise was not kept
             * 
             * This happens when no value was assigned to the promise before deconstruction.
             * If this happens an Exception (Derived from std::exception) will be thrown.
             */
            [[nodiscard]]inline bool is_broken_promise() const {return this->get_state() == State::BrokenPromise;}
    
            
        private:
            Promise<T> make_promise(){
                if(this->_promisePtr == nullptr){
                    return Promise<T>(this);
                }else{
                    throw Exception("Error: `Future::make_promise()` tried to create a second promise for the same future.");
                }
            }

        private:

            /// \brief Used by the Future to set the lock for a critical section that affect both the future and the promise
            /// \details Disables interrupts. Acquires own lock. Forcefully spins until the Promise lock becomes available.
            void acquire_dual_locks() {
                // return early - no need to lock if the future is detatched from the promise
                if(this->_promisePtr == nullptr){return;}
                
                embed::disable_interrupts();

                #ifndef EMBED_SINGLE_CORE
                    // lock its own lock
                    while (_lock.exchange(Lock::Closed, std::memory_order_acquire) != Lock::Open) {/* spin */}

                    // double check - because there might have been an interrupt between the first check and the disabling of the interrupt
                    if(this->_promisePtr == nullptr){
                        //cleanup if value changed

                        // release own lock
                        this->_lock.store(Lock::Open, std::memory_order_release);
                        
                        // re-enable interrupts
                        embed::enable_interrupts();
                        return;
                    }

                    // lock the promises lock
                    while (this->_promisePtr->_lock.exchange(Promise<T>::Lock::Closed, std::memory_order_acquire) != Promise<T>::Lock::Open) {/* spin */}
                #endif
                // successfully acquired both locks
            }
            
            /// \brief Used by the Future to set the lock for a critical section
            void release_dual_locks() {
                #ifndef EMBED_SINGLE_CORE
                    // release the lock of the promise
                    if(this->_promisePtr != nullptr){
                        this->_promisePtr->_lock.store(Promise<T>::Lock::Open,  std::memory_order_release);
                    }

                    // release the lock of this future
                    this->_lock.store(Lock::Open, std::memory_order_release);
                #endif
                embed::enable_interrupts();
            }

            /// \brief releases the locks and sets the pointers of the future and promise to each other to nullptr
            void release_dual_locks_and_detatch() {
                #ifndef EMBED_SINGLE_CORE
                    // release the lock of the promise
                    if(this->_promisePtr != nullptr){
                        this->_promisePtr->_futurePtr = nullptr;
                        this->_promisePtr->_lock.store(Promise<T>::Lock::Open,  std::memory_order_release);
                    }

                    // release the lock of this future
                    this->_lock.store(Lock::Open, std::memory_order_release);
                #else
                    this->_promisePtr->_futurePtr = nullptr;
                #endif
                embed::enable_interrupts();

                this->_promisePtr = nullptr;
            }
        };


    /**
     * \brief Promise and Future pairs are used to synchronise values between asynchronous tasks.
     * 
     * If you are on a single core bare-metal embedded system (aka. all context switching is interrupt driven)
     * you can set the `EMBED_SINGLE_CORE` definiteion flag for optimizations - smaller binary and faster execution. 
     * It will then remove all the locks and logic that is needed for multi-core thread safety.
     * 
     * Example: one wants to copy data asynchronously (for example with a DMA controller) while doing some computations in the mean time.
     * ```C++
     * 
     * // promise for CRC interrupts
     * static Promise<uint32_t> crc_promise;
     * 
     * // async function that accepts a promise for example when called from a future holder
     * embed::Future<uint32_t> async_crc(const void* data, size_t dataLength){
     * 
     *      // create a future promise pair use:
     *      auto [crc_future, crc_promise] = embed::make_future_promise<uint32_t>();
     * 
     *      // start the CRC calculation
     *      CRC_DMA_start(data, dataLength);
     * 
     *      // return before the CRC computation finished
     *      return crc_future;
     * } 
     * 
     * // Once the CRC computation finished the interrupt will be called
     * void crc_complete_interrupt(){
     *      
     *      // keep the promise by assigning value to it
     *      crc_promise.set_value(CRC->result_register);
     * }
     * 
     * int main(){
     *      // ...
     * 
     *      Future<uint32_t> crc_future = async_crc(message, length);
     * 
     *      // let the crc hardware do its thing and do some other math in the mean time
     *      // ... 
     *      // ... some math ...
     *      // ...
     * 
     *      // now we need the crc. The .get() method will either wait for the value from the intterupt befor returning it.
     *      send_message(message, length, crc_future.get());
     * }
     * 
     * ```
     */
    template<class T>
    class Promise{
    private:    

        friend class Future<T>;

        enum class Lock{
            Open, ///< no one has the lock
            Closed, ///< the lock is closed
        };    

        Future<T> *_futurePtr = nullptr;
        void (*_callback)(const T&) = nullptr; // optional callback

        #ifndef EMBED_SINGLE_CORE
            std::atomic<Lock> _lock = Lock::Open;
        #endif
    
        inline Promise(Future<T>* future) : _futurePtr(future){
            // acquire lock - no lock needed since this function will always be called in the same thread as the Future
            // register this promise
            this->_futurePtr->_promisePtr = this;
        }
    public:

        friend FuturePromisePair<T> make_future_promise<T>();

        Promise() = default;
        Promise(const Promise&) = delete;

        /// @brief Thread and interrupt save move save move that re-registers stack pointers in between the future and the promise
        Promise(Promise&& oldPromise){
            if(oldPromise._futurePtr != nullptr){
                // re-register the new object

                // acquire lock
                oldPromise._futurePtr->acquire_dual_locks();

                // set pointers
                this->_futurePtr = oldPromise._futurePtr;
                this->_futurePtr->_promisePtr = this;

                // release lock
                oldPromise._futurePtr->release_dual_locks();
            }
        }

        /// @brief Signals a broken promise to the future if no value has been assigned to the promise before destruction.
        inline ~Promise(){
            if(this->_futurePtr != nullptr){
                // acquire lock
                this->_futurePtr->acquire_dual_locks();

                // notify the future that the promise has been broken
                this->_futurePtr->set_state(Future<T>::State::BrokenPromise);

                // release lock and detatch (futurePtr, promisePtr, clallback = nullptr)
                this->_futurePtr->release_dual_locks_and_detatch();
            }
        }

        /// @brief Sets a value to the future that this promise is based on.
        /// @param obj The value that should be set to the Future and "keep the promise".
        /// @throws an std::exception on double writes
        inline void set_value(const T& value){
            if(this->_callback != nullptr){
                this->_callback(value);
            }else if(this->_futurePtr != nullptr){
                // acquire lock
                this->_futurePtr->acquire_dual_locks();

                // copy object
                this->_futurePtr->_value = value;
                this->_futurePtr->set_state(Future<T>::State::HasValue);
                
                // set the pointer to nullptr - because no bond is needed anymore
                this->_futurePtr->_promisePtr = nullptr;

                // release lock and detatch (futurePtr, promisePtr, clallback = nullptr)
                this->_futurePtr->release_dual_locks_and_detatch();
            }else{
                throw Exception("Error in `Promise::set_value(const T&)`: Double assignment to already kept promise.");
            }
        }

        /// @brief Sets a value to the future that this promise is based on.
        /// @param obj The value that should be set to the Future and "keep the promise"
        /// @throws an std::exception on double writes
        inline void set_value(T&& value){
            if(this->_callback != nullptr){
                this->_callback(value);
            }else if(this->_futurePtr != nullptr){
                // acquire lock
                this->_futurePtr->acquire_dual_locks();

                // move object
                this->_futurePtr->_value = std::move(value);
                this->_futurePtr->set_state(Future<T>::State::HasValue);

                // release lock and detatch (futurePtr, clallback = nullptr)
                this->_futurePtr->release_dual_locks_and_detatch();
            }else{
                throw Exception("Error in `Promise::set_value(const T&)`: Double assignment to already kept promise.");
            }
        }

    private:

        /// @brief locks its and the futures lock.
        /// @details Acquires its own lock. Attempts once to get the Future's lock. If it fails, releases its lock and retries, thereby giving Future a fair chance.
        void acquire_dual_locks(){
            // early return
            if(this->_promisePtr == nullptr){return;}
            
            // disable interrupts to prevent deadlocks
            embed::disable_interrupts();
            
            #ifndef EMBED_SINGLE_CORE
                bool Continue = true;

                while(Continue){
                    // lock its own lock
                    while (this->_lock.exchange(Lock::Closed, std::memory_order_acquire) != Lock::Open) {/* spin */}

                    // double check - because there might have been an interrupt before the first check and the disabling of the interrupts
                    if(this->_promisePtr == nullptr){
                        // cleanup if value changed

                        // open lock
                        this->_lock.store(Lock::Open, std::memory_order_release);

                        // enable interrupts
                        embed::enable_interrupts();

                        return;
                    }

                    // try to lock the other lock once, if failed, open both locks so that the future can get both locks, then re-try
                    if(this->_futurePtr->_lock.exchange(Future<T>::Lock::Closed, std::memory_order_acquire) != Future<T>::Lock::Open){
                        // release the lock so that the future may acquire both locks
                        this->_lock.store(Lock::Open, std::memory_order_release);

                        // failed to get the lock
                        Continue = true;
                    }else{
                        // succeeded in acquireing both locks
                        Continue = false;
                    }
                }
            #endif

            // successfully acquired both locks
        }

        /// \brief Used by the Future to set the lock for a critical section
        void release_dual_locks() {
            #ifndef EMBED_SINGLE_CORE
                // release the lock of the future
                if(this->_futurePtr != nullptr){
                    this->_futurePtr->_lock.store(Future<T>::Lock::Open,  std::memory_order_release);
                }

                // release the lock of this promise
                this->_lock.store(Lock::Open, std::memory_order_release);
            #endif
            embed::enable_interrupts();
        }

        /// \brief releases the locks and sets the pointers of the future and promise to each other to nullptr
        void release_dual_locks_and_detatch() {
            #ifndef EMBED_SINGLE_CORE
                // release the lock of the future
                if(this->_futurePtr != nullptr){
                    // delete the promises connection to their future before enabling it
                    this->_futurePtr->_futurePtr = nullptr;

                    // unlock the future
                    this->_futurePtr->_lock.store(Future<T>::Lock::Open,  std::memory_order_release);
                }
                
                // release the lock of this promise
                this->_lock.store(Lock::Open, std::memory_order_release);
            #else
                this->_futurePtr->_futurePtr = nullptr;
            #endif

            embed::enable_interrupts();

            // detatch this
            this->_promisePtr = nullptr;
            this->_callback = nullptr;
        }

    };
}// namespace embed

namespace embed{
    template<class T>
    struct FuturePromisePair{
        Future<T> future;
        Promise<T> promise;
    };
}

// make Future Promise Tuple decomposable:
// specialize the tuple traits:
namespace std {
    template<class T> struct tuple_size<embed::FuturePromisePair<T>> : std::integral_constant<std::size_t, 2> {};
    template<class T> struct tuple_element<0, embed::FuturePromisePair<T>> { using type = embed::Future<T>; };
    template<class T> struct tuple_element<1, embed::FuturePromisePair<T>> { using type = embed::Promise<T>; };
} // namespace std




namespace embed{

    // provide `get<>()`
    template<std::size_t I, class T>
    decltype(auto) get(embed::FuturePromisePair<T>&& p){
        if constexpr (I == 0){
            return std::move(p.future);
        }else{
            return std::move(p.promise);
        }
    }

    /**
     * @brief creates a linked future promise pair
     * 
     * Example:
     * ```
     * auto [future, promise] = make_future_promise<int>();
     * ```
     * 
     * @tparam T The type that is being promised by the promise and awaited by the future
     * @returns A linked `FuturePromisePair` Future promise pair
     */
    template<class T>
    FuturePromisePair<T> make_future_promise(){
        // create pair
        FuturePromisePair<T> pair;

        // link pair
        pair.future._promisePtr = &pair.promise;
        pair.promise._futurePtr = &pair.future;

        // return pair
        return pair;
    }

} // namespace embed