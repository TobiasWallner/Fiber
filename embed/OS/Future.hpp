#pragma once

// std
#ifndef EMBED_SINGLE_CORE
    #include <atomic>
#endif
#include <coroutine>
#include <tuple>
#include <optional>

// embed
#include <embed/Exceptions/Exceptions.hpp>
#include <embed/interrupts/interrupts.hpp>
#include <embed/OS/Coroutine.hpp>

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
    class Future : public AwaitableNode{
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
                std::atomic<State> _state = State::BrokenPromise; // volatile because this will be set by a different threat.
                std::atomic<Lock> _lock = Lock::Open;
            #else
                volatile State _state = State::BrokenPromise; // volatile because this will be set by a different threat.
            #endif

            friend class Promise<T>;
        public:
    
            friend FuturePromisePair<T> make_future_promise<T>();

            Future() = default;
            Future(const Future&) = delete;
            Future& operator=(const Future&) = delete;

            Future& operator=(Future&& oldFuture){
                if(this != &oldFuture){
                    // detatch this
                    if(this->_promisePtr){
                        this->acquire_dual_locks();
                        this->detatch_release_dual_locks();
                    }

                    switch(oldFuture.state()){
                        case State::Busy:{
                            // aquire old locks
                            oldFuture.acquire_dual_locks();
                            this->_promisePtr = oldFuture._promisePtr;
                            this->set_state(oldFuture.state()) ;
                            this->_value = std::move(oldFuture._value);

                            // re-register
                            if(this->is_waiting() && (this->_promisePtr != nullptr)){
                                this->_promisePtr->_futurePtr = this;
                            }

                            // transfere locks
                            this->_lock.store(oldFuture._lock.load(std::memory_order_acquire), std::memory_order_release);

                            // invalidate oldFuture
                            oldFuture._promisePtr = nullptr;
                            oldFuture.set_state(State::BrokenPromise);
                            oldFuture._lock.store(Lock::Open, std::memory_order_release);

                            // handover complete, this has to release the lock now
                            this->release_dual_locks();
                        }break;
                        case State::HasValue : {
                            this->_value = std::move(oldFuture._value);
                            this->set_state(State::HasValue);
                        }break;
                        case State::BrokenPromise : {
                            this->set_state(State::BrokenPromise);
                        }break;
                    }
                    
                }
                return *this;
            }
            /// @brief Thread and interrupt save move that re-registers stack pointer in between the future and the promise
            Future(Future&& oldFuture){
                
                switch(oldFuture.state()){
                    case State::Busy:{
                        // aquire old locks
                        oldFuture.acquire_dual_locks();
                        this->_promisePtr = oldFuture._promisePtr;
                        this->set_state(oldFuture.state()) ;
                        this->_value = std::move(oldFuture._value);

                        // re-register
                        if(this->is_waiting() && (this->_promisePtr != nullptr)){
                            this->_promisePtr->_futurePtr = this;
                        }

                        // transfere locks
                        this->_lock.store(oldFuture._lock.load(std::memory_order_acquire), std::memory_order_release);

                        // invalidate oldFuture
                        oldFuture._promisePtr = nullptr;
                        oldFuture.set_state(State::BrokenPromise);
                        oldFuture._lock.store(Lock::Open, std::memory_order_release);

                        // handover complete, this has to release the lock now
                        this->release_dual_locks();
                    }break;
                    case State::HasValue : {
                        this->_value = std::move(oldFuture._value);
                        this->set_state(State::HasValue);
                    }break;
                    case State::BrokenPromise : {
                        this->set_state(State::BrokenPromise);
                    }break;
                }
                    
                
                
            }

            ~Future(){
                if(this->_promisePtr){
                    // detatch from the promise
                    this->acquire_dual_locks();
                    this->detatch_release_dual_locks();
                }
            }

            inline bool is_connected() const {return this->_promisePtr != nullptr;}
            inline bool is_detatched() const {return this->_promisePtr == nullptr;}

            /**
             * @brief Checks if the promise is connected to the future
             * @returns `true` if the passed promise is the one connected to this future
             */
            inline bool is_connected_to(const Promise<T>& promise) const {
                const bool result = this->_promisePtr == &promise;
                return result;
            }

            /**
             * \brief Blocks the current thread until the value is ready or an error occured.
             */
            void wait() {while(this->is_waiting()){/* Do Nothing */}}
    
            /**
             * \brief returns the value of the future and waits if necessary
             * \throws Exception of type embed::Exception if an error occured.
             */
            [[nodiscard]]T& get(){
                this->wait();
                if(this->is_ready()){
                    return this->_value;
                }else{
                    EMBED_THROW(Exception("Read from broken promise."));
                }
            }

            /**
             * \brief returns the value of the future and waits if necessary. If the value does not exist, a `nullptr` is being returned
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
        
            inline State state() const {
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
            [[nodiscard]]constexpr bool is_ready() const {return this->get_state() == State::HasValue;}

            /// \brief converts to a boolean. same as calling `is_ready()`.
            [[nodiscard]]constexpr operator bool() const {return this->is_ready();}

            /// @brief returns true if the result is not finished yet and one has to wait
            [[nodiscard]]constexpr bool is_waiting() const {return this->get_state() == State::Busy;}
    
            /**
             * \brief return true if the promise was not kept
             * 
             * This happens when no value was assigned to the promise before deconstruction.
             * If this happens an Exception (Derived from std::exception) will be thrown.
             */
            [[nodiscard]]constexpr bool is_broken_promise() const {return this->get_state() == State::BrokenPromise;}
    
            /**
             * @brief `co_await` interoperability, returns true, if the future is no longer waiting.
             * 
             * Returns true if the future is not waiting anymore. Note that this does not mean that it
             * has a readable value. It could also be broken, because the promise died before it could write 
             * a value
             * 
             * @return `true` if the Future is no longer waiting on the Promise
             */
            constexpr bool await_ready() const noexcept override {
                return !this->is_waiting();
            }
            
            /**
             * @brief `co_await` interoperability and optionally returns a value if one has been set.
             * 
             * @returns returns the value if the value has been set and `is_ready()` would also return `true`, returns a `std::nullopt` otherwise.
             */
            std::optional<T> await_resume() noexcept {
                if(this->is_ready()){
                    return this->_value;
                }else{
                    return std::nullopt;
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
            void detatch_release_dual_locks() {
                #ifndef EMBED_SINGLE_CORE
                    // release the lock of the promise
                    if(this->_promisePtr != nullptr){
                        this->_promisePtr->_futurePtr = nullptr; /// !!! only time allowed to clear the future pointer
                        this->_promisePtr->_lock.store(Promise<T>::Lock::Open,  std::memory_order_release);
                        
                        // release the lock of this future
                        this->_lock.store(Lock::Open, std::memory_order_release);

                        // re-enable interrupts
                        embed::enable_interrupts();
                    }

                #else
                    if(this->_promisePtr != nullptr){
                        this->_promisePtr->_futurePtr = nullptr;
                        embed::enable_interrupts();
                    }
                #endif

                this->_promisePtr = nullptr; // !!! only time allowed to clear the promise pointer
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
        Promise& operator=(Promise&& oldPromise){
            if(this != &oldPromise){
                // detatch this
                if(oldPromise._futurePtr){
                    this->acquire_dual_locks();
                    this->detatch_release_dual_locks();

                    // lock old
                    oldPromise.acquire_dual_locks();

                    // copy data from oldPromise
                    this->_futurePtr = oldPromise._futurePtr;
                    
                    // re-register
                    if(this->_futurePtr){
                        this->_futurePtr->_promisePtr = this;
                    }
                    
                    // transfere locks
                    this->_lock.store(oldPromise._lock.load(std::memory_order_acquire), std::memory_order_release);
                    
                    // invalidate oldPromise
                    oldPromise._futurePtr = nullptr;
                    oldPromise._lock.store(Lock::Open);

                    // handover complete release this
                    this->release_dual_locks();
                }else{
                    this->_futurePtr = nullptr;
                }
                
            }
            return *this;
        }

        Promise(Promise&& oldPromise){
            if(oldPromise._futurePtr){
                // lock old
                oldPromise.acquire_dual_locks();

                // copy data from oldPromise
                this->_futurePtr = oldPromise._futurePtr;
                
                // re-register
                if(this->_futurePtr){
                    this->_futurePtr->_promisePtr = this;
                }
                
                // transfere locks
                this->_lock.store(oldPromise._lock.load(std::memory_order_acquire), std::memory_order_release);
                
                // invalidate oldPromise
                oldPromise._futurePtr = nullptr;
                oldPromise._lock.store(Lock::Open);

                // handover complete release this
                this->release_dual_locks();
            }else{
                this->_futurePtr = nullptr;
            }
        }

        

        /**
         * @brief Checks if the passed future is the one connected to this promise
         * @returns `true` if the passed future is the same that this promise is connected to.
         *  */ 
        bool is_connected_to(const Future<T>& future) const {
            const bool result = this->_futurePtr == &future;
            return result;
        }

        /// @brief Signals a broken promise to the future if no value has been assigned to the promise before destruction.
        inline ~Promise() noexcept {
            if(this->_futurePtr){
                // acquire lock
                this->acquire_dual_locks();

                // double-check
                if(this->_futurePtr != nullptr){
                    // notify the future that the promise has been broken
                    this->_futurePtr->set_state(Future<T>::State::BrokenPromise);
                }

                // release lock and detatch (futurePtr, promisePtr, clallback = nullptr)
                this->detatch_release_dual_locks();
            }
        }

        /// @brief Sets a value to the future that this promise is based on.
        /// @param value The value that should be set to the Future and "keep the promise".
        /// @throws an std::exception on double writes
        void set_value(const T& value){
            if(this->_futurePtr != nullptr){
                // acquire lock
                this->acquire_dual_locks();

                // double check
                if(this->_futurePtr != nullptr){
                    // copy object
                    this->_futurePtr->_value = value;
                    this->_futurePtr->set_state(Future<T>::State::HasValue);
                }

                // release lock and detatch (futurePtr, promisePtr, clallback = nullptr)
                this->detatch_release_dual_locks();
            }else{
                EMBED_THROW(Exception("Double assignment to already kept promise."));
            }
        }

        /// @brief equivalent to `set_value()` 
        Promise& operator=(const T& value){
            this->set_value(value);
            return *this;
        }

        /// @brief Sets a value to the future that this promise is based on.
        /// @param value The value that should be set to the Future and "keep the promise"
        /// @throws an embed::Exception on double writes
        void set_value(T&& value){
            if(this->_futurePtr != nullptr){
                
                // acquire lock
                this->acquire_dual_locks();

                // double check
                if(this->_futurePtr != nullptr){
                    // move object
                    this->_futurePtr->_value = std::move(value);
                    this->_futurePtr->set_state(Future<T>::State::HasValue);
                }

                // release lock and detatch (futurePtr, clallback = nullptr)
                this->detatch_release_dual_locks();
            }else{
                EMBED_THROW(Exception("Double assignment to already kept promise."));
            }
        }

        /// @brief equivalent to `set_value()` 
        Promise& operator=(T&& value){
            this->set_value(std::move(value));
            return *this;
        }

    private:

        /// @brief locks its and the futures lock.
        /// @details Acquires its own lock. Attempts once to get the Future's lock. If it fails, releases its lock and retries, thereby giving Future a fair chance.
        void acquire_dual_locks(){
            // early return
            if(this->_futurePtr == nullptr){return;}
            
            // disable interrupts to prevent deadlocks
            embed::disable_interrupts();
            
            #ifndef EMBED_SINGLE_CORE
                

                while(true){
                    // lock its own lock
                    while (this->_lock.exchange(Lock::Closed, std::memory_order_acquire) != Lock::Open) {/* spin */}

                    // double check - because there might have been an interrupt before the first check and the disabling of the interrupts
                    if(this->_futurePtr == nullptr){
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

                        // failed to get the lock --> contine loop and retry
                        continue;
                    }else{
                        // succeeded in acquireing both locks --> stop trying and exit loop
                        break;
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
        void detatch_release_dual_locks() {
            #ifndef EMBED_SINGLE_CORE
                // release the lock of the future
                if(this->_futurePtr != nullptr){
                    // delete the promises connection before opening the lock
                    this->_futurePtr->_promisePtr = nullptr;

                    // unlock the future
                    this->_futurePtr->_lock.store(Future<T>::Lock::Open,  std::memory_order_release);
                    this->_lock.store(Lock::Open, std::memory_order_release);
                    embed::enable_interrupts();
                }
                
                // release the lock of this promise
            #else
                if(this->_futurePtr != nullptr){
                    this->_futurePtr->_promisePtr = nullptr;
                    this->_lock.store(Lock::Open, std::memory_order_release);
                    embed::enable_interrupts();
                }
            #endif


            // detatch this
            this->_futurePtr = nullptr;
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
        pair.future.set_state(Future<T>::State::Busy);
        pair.promise._futurePtr = &pair.future;

        // return pair
        return pair;
    }

} // namespace embed