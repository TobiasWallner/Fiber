#pragma once

#include "DualArrayList.hpp"

namespace fiber
{

    
    template<class T, size_t N, class stage1_less_priority, class stage2_less_priority>
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
        using stage2_iterator = DualArrayList<T, N>::left_iterator;
        using stage2_const_iterator = DualArrayList<T, N>::left_const_iterator;
        using stage1_iterator = DualArrayList<T, N>::right_iterator;
        using stage1_const_iterator = DualArrayList<T, N>::right_const_iterator;

    private:
        RightDualArrayListRef<T, N> stage1(){return this->_buffer;}
        LeftDualArrayListRef<T, N> stage2(){return this->_buffer;}

    public:

        RightDualArrayListConstRef<T, N> stage1() const {return this->_buffer;}
        LeftDualArrayListConstRef<T, N> stage2() const {return this->_buffer;}

        constexpr stage1_iterator stage1_begin() {return this->stage1().begin();}
        constexpr stage1_const_iterator stage1_begin() const {return this->stage1().begin();}
        constexpr stage1_const_iterator stage1_cbegin() const {return this->stage1().cbegin();}

        constexpr stage2_iterator stage2_begin() {return this->stage2().begin();}
        constexpr stage2_const_iterator stage2_begin() const {return this->stage2().begin();}
        constexpr stage2_const_iterator stage2_cbegin() const {return this->stage2().cbegin();}

        constexpr stage1_iterator stage1_end() {return this->stage1().end();}
        constexpr stage1_const_iterator stage1_end() const {return this->stage1().end();}
        constexpr stage1_const_iterator stage1_cend() const {return this->stage1().cend();}

        constexpr stage2_iterator stage2_end() {return this->stage2().end();}
        constexpr stage2_const_iterator stage2_end() const {return this->stage2().end();}
        constexpr stage2_const_iterator stage2_cend() const {return this->stage2().cend();}

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
            std::push_heap(this->stage1().begin(), this->stage1().end(), stage1_less_priority{});
        }

        void stage1_push(T&& value){
            this->stage1().emplace_back(std::move(value));
            std::push_heap(this->stage1().begin(), this->stage1().end(), stage1_less_priority{});
        }

        void stage2_push(const T& value){
            this->stage2().emplace_back(value);
            std::push_heap(this->stage2().begin(), this->stage2().end(), stage2_less_priority{});
        }

        void stage2_push(T&& value){
            this->stage2().emplace_back(std::move(value));
            std::push_heap(this->stage2().begin(), this->stage2().end(), stage2_less_priority{});
        }

        void stage2_pop(){
            std::pop_heap(this->stage2().begin(), this->stage2().end(), stage2_less_priority{});
            this->stage2().pop_back();
        }

        void stage1_pop(){
            std::pop_heap(this->stage1().begin(), this->stage1().end(), stage2_less_priority{});
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

    };    

    template<class T, size_t N, class stage1_less_priority, class stage2_less_priority>
    class Stage1DualPriorityQueueConstRef{
        private:
        using BaseType = DualPriorityQueue<T, N, stage1_less_priority, stage2_less_priority>;
        const BaseType& _queue;

        public:
        using size_type = BaseType::size_type;
        using reference = BaseType::reference;
        using const_reference = BaseType::const_reference;
        using pointer = BaseType::pointer;
        using const_pointer = BaseType::const_pointer;
        using iterator = BaseType::stage1_iterator;
        using const_iterator = BaseType::stage1_const_iterator;

        Stage1DualPriorityQueueConstRef(const DualPriorityQueue<T, N, stage1_less_priority, stage2_less_priority>& queue)
            : _queue(queue){}

        constexpr const_iterator begin() const {return this->_queue.stage1_begin();}
        constexpr const_iterator cbegin() const {return this->_queue.stage1_cbegin();}

        constexpr const_iterator end() const {return this->_queue.stage1_end();}
        constexpr const_iterator cend() const {return this->_queue.stage1_cend();}

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
        constexpr const_reference top() const {return this->_queue.stage1_top();}
    };  

    template<class T, size_t N, class stage1_less_priority, class stage2_less_priority>
    class Stage1DualPriorityQueueRef{
        private:
        using BaseType = DualPriorityQueue<T, N, stage1_less_priority, stage2_less_priority>;
        BaseType& _queue;


        public:
        using size_type = BaseType::size_type;
        using reference = BaseType::reference;
        using const_reference = BaseType::const_reference;
        using pointer = BaseType::pointer;
        using const_pointer = BaseType::const_pointer;
        using iterator = BaseType::stage1_iterator;
        using const_iterator = BaseType::stage1_const_iterator;

        Stage1DualPriorityQueueRef(DualPriorityQueue<T, N, stage1_less_priority, stage2_less_priority>& queue)
            : _queue(queue){}


        constexpr iterator begin() {return this->_queue.stage1_begin();}
        constexpr const_iterator begin() const {return this->_queue.stage1_begin();}
        constexpr const_iterator cbegin() const {return this->_queue.stage1_cbegin();}

        constexpr iterator end() {return this->_queue.stage1_end();}
        constexpr const_iterator end() const {return this->_queue.stage1_end();}
        constexpr const_iterator cend() const {return this->_queue.stage1_cend();}
        

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
        constexpr reference top() {return this->_queue.stage1_top();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr const_reference top() const {return this->_queue.stage1_top();}


        inline void push(const T& value){return this->_queue.stage1_push(value);}

        inline void push(T&& value){return this->_queue.stage1_push(std::move(value));}


        inline void pop(){return this->_queue.stage1_pop();}

        inline T top_pop(){return this->_queue.stage1_top_pop();}
    };  

    template<class T, size_t N, class stage1_less_priority, class stage2_less_priority>
    class Stage2DualPriorityQueueConstRef{
        private:
        using BaseType = DualPriorityQueue<T, N, stage1_less_priority, stage2_less_priority>;
        const BaseType& _queue;

        public:
        using size_type = BaseType::size_type;
        using reference = BaseType::reference;
        using const_reference = BaseType::const_reference;
        using pointer = BaseType::pointer;
        using const_pointer = BaseType::const_pointer;
        using iterator = BaseType::stage2_iterator;
        using const_iterator = BaseType::stage2_const_iterator;

        Stage2DualPriorityQueueConstRef(const DualPriorityQueue<T, N, stage1_less_priority, stage2_less_priority>& queue)
            : _queue(queue){}

        constexpr const_iterator begin() const {return this->_queue.stage2_begin();}
        constexpr const_iterator cbegin() const {return this->_queue.stage2_cbegin();}

        constexpr const_iterator end() const {return this->_queue.stage2_end();}
        constexpr const_iterator cend() const {return this->_queue.stage2_cend();}

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
        constexpr const_reference top() const {return this->_queue.stage2_top();}
    };


    template<class T, size_t N, class stage1_less_priority, class stage2_less_priority>
    class Stage2DualPriorityQueueRef{
        private:
        using BaseType = DualPriorityQueue<T, N, stage1_less_priority, stage2_less_priority>;
        BaseType& _queue;

        public:
        using size_type = BaseType::size_type;
        using reference = BaseType::reference;
        using const_reference = BaseType::const_reference;
        using pointer = BaseType::pointer;
        using const_pointer = BaseType::const_pointer;
        using iterator = BaseType::stage2_iterator;
        using const_iterator = BaseType::stage2_const_iterator;

        Stage2DualPriorityQueueRef(DualPriorityQueue<T, N, stage1_less_priority, stage2_less_priority>& queue)
            : _queue(queue){}

        constexpr iterator begin() {return this->_queue.stage2_begin();}
        constexpr const_iterator begin() const {return this->_queue.stage2_begin();}
        constexpr const_iterator cbegin() const {return this->_queue.stage2_cbegin();}

        constexpr iterator end() {return this->_queue.stage2_end();}
        constexpr const_iterator end() const {return this->_queue.stage2_end();}
        constexpr const_iterator cend() const {return this->_queue.stage2_cend();}

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
        constexpr reference top() {return this->_queue.stage2_top();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr const_reference top() const {return this->_queue.stage2_top();}


        inline void push(const T& value){return this->_queue.stage2_push(value);}

        inline void push(T&& value){return this->_queue.stage2_push(std::move(value));}


        inline void pop(){return this->_queue.stage2_pop();}

        inline T top_pop(){return this->_queue.stage2_top_pop();}
    };


} //namespace fiber

