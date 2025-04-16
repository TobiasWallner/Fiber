#pragma once

#include "DualArrayList.hpp"

namespace embed
{

    /**
     * @brief a dual stage priority list.
     * 
     * Stage 1 is sorted using `stage1_greater_priority()`.
     * Elements transition from stage 1 to stage 2 if they pass the `transition_condition()`
     * Stage 2 is sorted using the `stage2_greater_priority()`.
     * `.top()` then returns the top priority element of stage 2
     * 
     * Elements are emplaced into stage 1 if the `transition_condition()` fails and directly into stage 2 if it passes
     * 
     * @tparam T The type of the elements in the queues
     * @tparam N The maximum number of elements for both stages combined
     * @tparam stage1_greater_priority A priority function for the first stage that returns true if the first argument has a greater priority than the second one
     * @tparam stage2_greater_priority A priority function for the second stage that returns true if the first argument has a greater priority than the second one
     * @tparam transition_condition The condition to promote elements from the first stage into the second stage
     */
    template<class T, size_t N, class stage1_greater_priority, class stage2_greater_priority, class transition_condition>
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

        /// @brief returns the capacity of the container. Since this is a statically allocated container this is also the maximal size.
        constexpr size_type capacity() const {return this->_buffer.max_size();}

        /// @brief returns the maximal number of elements that can be stored in the container 
        constexpr size_type max_size() const {return this->_buffer.max_size();}

        /// @brief returns the reserve - number of elements that can be stored until the container is full 
        constexpr size_type reserve() const {return this->_buffer.reserve();}

        /// @brief returns true if there are not elements in the container, aka. the container is empty. 
        constexpr bool empty() const {return this->_buffer.empty();}

        /// @brief returns true if the container is full and no more elements can be stored in the container 
        constexpr bool full() const {return this->_buffer.full();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr reference top() {return this->stage2().front();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr const_reference top() const {return this->stage2().front();}

        /// @brief promotes/transitions all consecutive top elements of stage 1 that pass the transition condition into stage 2
        void promote(){
            while(transition_condition(this->stage1().front())){
                // get first element from stage 1 and restore its heap
                value_type temp = std::move(this->stage1().front());
                std::ranges::pop_heap(this->stage1(), stage1_greater_priority);
                this->stage1().pop_back();

                // emplace it into stage 2 and restore its heap
                this->stage2().emplace_back(std::move(temp));
                std::ranges::push_heap(this->stage2(), stage2_greater_priority);
            }
        }

        /**
         * @brief Adds a value into its corresponding stage in the heap
         * 
         * If a value fullfills the `transition_condition()`, it will be placed directly into the
         * stage 2 heap, skipping stage 1.
         * 
         * If a value does not fulfull the `transition_condition()`, it will be placed into the stage 1 heap
         * 
         * > Note: that `.push()` does not promote any other elements from stage 1 into stage 2
         * 
         * @param value the value to be inserted
         */
        void push(const T& value){
            if(transition_condition(value)){
                this->stage2().emplace_back(value);
                std::ranges::push_heap(this->stage2(), stage2_greater_priority);
            }else{
                this->stage1().emplace_back(value);
                std::ranges::push_heap(this->stage1(), stage1_greater_priority);
            }
        }

        /**
         * @brief removes the top (from stage 2) and re-establishes its heap
         * 
         * > Note: this function does not promote any values from stage 1 to stage 2
         */
        void pop(){
            std::ranges::pop_heap(this->stage2(), stage2_greater_priority);
            this->stage2().pop_back();
        }

        /**
         * @brief returns the top most element (from stage 2) and removes it from the heap
         */
        T top_pop(){
            T result = std::move(this->top());
            this->pop();
            return result;
        }

    }    
} //namespace embed

