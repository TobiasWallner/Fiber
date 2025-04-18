#pragma once

#include "DualArrayList.hpp"

namespace embed
{

    
    template<class T, size_t N, class stage1_greater_priority, class stage2_greater_priority>
    class DualPriorityQueue{
    private:
        DualArrayList<T, N> _buffer;
    
    public:
        
        using value_type = DualArrayList<T, N>::value_type;
        using size_type = DualArrayList<T, N>::size_type;
        using reference = DualArrayList<T, N>::reference;
        using const_reference = DualArrayList<T, N>::const_reference;
        using pointer = DualArrayList<T, N>::pointer;
        using const_pointer = DualArrayList<T, N>::const_pointer;

    private:
        RightDualArrayListRef stage1(){return this->_buffer;}
        LeftDualArrayListRef stage2(){return this->_buffer;}

    public:

        /// @brief returns a const/immutable reference to the first stage
        RightDualArrayListConstRef stage1() const {return this->_buffer;}

        /// @brief returns const/immutable reference to the second stage 
        LeftDualArrayListConstRef stage2() const {return this->_buffer;}

        /// @brief returns the size/count of live elements in the container 
        constexpr size_type size() const {return this->_buffer.size();}

        constexpr size_type stage1_size() const {return this->stage1().size();}

        constexpr size_type stage2_size() const {return this->stage2().size();}

        /// @brief returns the capacity of the container. Since this is a statically allocated container this is also the maximal size.
        constexpr size_type stage1_capacity() const {return this->stage1().capacity();}
        constexpr size_type stage2_capacity() const {return this->stage2().capacity();}

        /// @brief returns the maximal number of elements that can be stored in the container 
        constexpr size_type max_size() const {return this->_buffer.max_size();}

        /// @brief returns the reserve - number of elements that can be stored until the container is full 
        constexpr size_type reserve() const {return this->_buffer.reserve();}

        /// @brief returns true if there are not elements in the container, aka. the container is empty. 
        constexpr bool empty() const {return this->_buffer.empty();}
        constexpr bool stage1_empty() const {return this->stage1().empty();}
        constexpr bool stage2_empty() const {return this->stage2().empty();}

        /// @brief returns true if the container is full and no more elements can be stored in the container 
        constexpr bool full() const {return this->_buffer.full();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr reference stage1_top() {return this->stage1().front();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr const_reference stage1_top() const {return this->stage1().front();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr reference stage2_top() {return this->stage2().front();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr const_reference stage2_top() const {return this->stage2().front();}

        void stage1_push(const T& value){
            this->stage1().emplace_back(value);
            std::ranges::push_heap(this->stage1(), stage1_greater_priority);
        }

        void stage1_push(T&& value){
            this->stage1().emplace_back(std::move(value));
            std::ranges::push_heap(this->stage1(), stage1_greater_priority);
        }

        void stage2_push(const T& value){
            this->stage2().emplace_back(value);
            std::ranges::push_heap(this->stage2(), stage2_greater_priority);
        }

        void stage2_push(T&& value){
            this->stage2().emplace_back(std::move(value));
            std::ranges::push_heap(this->stage2(), stage2_greater_priority);
        }


        void stage2_pop(){
            std::ranges::pop_heap(this->stage2(), stage2_greater_priority);
            this->stage2().pop_back();
        }

        void stage1_pop(){
            std::ranges::pop_heap(this->stage1(), stage2_greater_priority);
            this->stage1().pop_back();
        }


        T stage1_top_pop(){
            T result = std::move(this->stage1_top());
            this->stage1_pop();
            return result;
        }

        T stage2_top_pop(){
            T result = std::move(this->stage2_top());
            this->stage2_pop();
            return result;
        }

    }    


    template<class T, size_t N, class stage1_greater_priority, class stage2_greater_priority>
    class Stage1DualPriorityQueue{
        private:
        DualPriorityQueue<T, N, stage1_greater_priority, stage2_greater_priority>& _queue;

        public:
        DualPriorityQueue1(DualPriorityQueue<T, N, stage1_greater_priority, stage2_greater_priority>& queue)
            : _queue(queue){}

        /// @brief returns the size/count of live elements in the container 
        constexpr size_type size() const {return this->_queue.stage1_size();}

        /// @brief returns the capacity of the container. Since this is a statically allocated container this is also the maximal size.
        constexpr size_type capacity() const {return this->_queue.stage1_capacity();}

        /// @brief returns the maximal number of elements that can be stored in the container 
        constexpr size_type max_size() const {return this->_queue.max_size();}

        /// @brief returns the reserve - number of elements that can be stored until the container is full 
        constexpr size_type reserve() const {return this->_queue.reserve();}

        /// @brief returns true if there are not elements in the container, aka. the container is empty. 
        constexpr bool empty() const {return this->_queue.stage1_empty();}

        /// @brief returns true if the container is full and no more elements can be stored in the container 
        constexpr bool full() const {return this->_queue.full();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr reference top() {return this->_queue.stage1_top()();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr const_reference top() const {return this->_queue.stage1_top();}


        inline void push(const T& value){return this->_queue.stage1_push(value);}

        inline void push(T&& value){return this->_queue.stage1_push(std::move(value));}


        inline void pop(){return this->_queue.stage1_pop();}

        inline T top_pop(){return this->_queue.stage1_top_pop();}
    };  


    template<class T, size_t N, class stage1_greater_priority, class stage2_greater_priority>
    class Stage2DualPriorityQueue{
        private:
        DualPriorityQueue<T, N, stage1_greater_priority, stage2_greater_priority>& _queue;

        public:
        Stage2DualPriorityQueue(DualPriorityQueue<T, N, stage1_greater_priority, stage2_greater_priority>& queue)
            : _queue(queue){}

        /// @brief returns the size/count of live elements in the container 
        constexpr size_type size() const {return this->_queue.stage2_size();}

        /// @brief returns the capacity of the container. Since this is a statically allocated container this is also the maximal size.
        constexpr size_type capacity() const {return this->_queue.stage2_capacity();}

        /// @brief returns the maximal number of elements that can be stored in the container 
        constexpr size_type max_size() const {return this->_queue.max_size();}

        /// @brief returns the reserve - number of elements that can be stored until the container is full 
        constexpr size_type reserve() const {return this->_queue.reserve();}

        /// @brief returns true if there are not elements in the container, aka. the container is empty. 
        constexpr bool empty() const {return this->_queue.stage2_empty();}

        /// @brief returns true if the container is full and no more elements can be stored in the container 
        constexpr bool full() const {return this->_queue.full();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr reference top() {return this->_queue.stage2_top()();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr const_reference top() const {return this->_queue.stage2_top();}


        inline void push(const T& value){return this->_queue.stage2_push(value);}

        inline void push(T&& value){return this->_queue.stage2_push(std::move(value));}


        inline void pop(){return this->_queue.stage2_pop();}

        inline T top_pop(){return this->_queue.stage2_top_pop();}
    };
} //namespace embed

