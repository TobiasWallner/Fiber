#pragma once


// std
#include <cstddef> // size_type
#include <algorithm> // swap

// embed
#include "StaticArrayList.hpp"

namespace embed
{
    template<class T, size_type N>
    class StaticPriorityQueue{
    public:
        using comntainer_type = StaticArrayList<T, N>;
    
    private:
        bool (*_greater_priority)(const T& lhs, const T& rhs);
        StaticArrayList<T, N> _buffer;

    public:

        using value_type = comntainer_type::value_type;
        using size_type = comntainer_type::size_type;
        using reference = comntainer_type::reference;
        using const_reference = comntainer_type::const_reference;
        using iterator = comntainer_type::iterator;
        using const_iterator = comntainer_type::const_iterator;

        StaticPriorityQueue(bool (*greater_priority)(const T& lhs, const T& rhs)) : _greater_priority(greater_priority){}
        StaticPriorityQueue(const StaticPriorityQueue&) = default;
        StaticPriorityQueue& operator=(const StaticPriorityQueue&) = default;
        
        /// @brief returns the size/count of live elements in the container 
        constexpr size_type size() const {return this->_buffer.size();}

        /// @brief returns the capacity of the container. Since this is a statically allocated container this is also the maximal size.
        constexpr size_type capacity() const {return this->_buffer.capacity();}

        /// @brief returns the maximal number of elements that can be stored in the container 
        constexpr size_type max_size() const {return this->_buffer.max_size();}

        /// @brief returns the reserve - number of elements that can be stored until the container is full 
        constexpr size_type reserve() const {return this->_buffer.reserve();}

        /// @brief returns true if there are not elements in the container, aka. the container is empty. 
        constexpr bool empty() const {return this->_buffer.empty();}

        /// @brief returns true if the container is full and no more elements can be stored in the container 
        constexpr bool full() const {return this->_buffer.full();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr reference top() {return this->_buffer.front();}

        /// @brief returns a reference to the element with the top most priority 
        constexpr const_reference top() const {return this->_buffer.front();}

        template<class... Args>
        reference emplace(Args&&... args){
            this->_buffer.emplace_back(std::forward<Args>(args)...);
            return this->sort_up(this->size() - 1);
        }

        void pop(){
            size_type parent_idx = 0;
            size_type max_child = this->max_child(parent_idx);

            // overwrite the first element with the last one and destroy it
            this->_buffer.front() = std::move(this->_buffer.back());
            this->_buffer.pop_back();

            // swap all parents with the higher priority child
            while(max_child < this->size()){
                // swap child with parent
                std::swap(this->_buffer[parent_idx], this->_buffer[max_child]);
        
                // calculate next chil
                parent_idx = max_child;
                max_child = this->max_child(max_child);
            }
        }

        value_type get_top{
            value_type value = std::move(this->top());
            this->pop();
            return value;
        }

    private:

        [[nodiscard]] static constexpr size_type left_child(size_type i){return i * 2 + 1;}
        [[nodiscard]] static constexpr size_type right_child(size_type i){return i * 2 + 2;}
        [[nodiscard]] static constexpr size_type parent(size_type i){return (i-1) / 2;}

        [[nodiscard]] size_type max_child(const size_type parent_idx){
            const size_type left_idx = left_child(parent_idx);
            if(left_idx >= this->size()) return left_idx;
            const size_type right_idx = right_child(parent_idx);
            if(right_idx >= this->size()) return left_idx;
            if(this->_greater_priority(this->_buffer[left_idx], this->_buffer[right_idx])){
                return left_idx;
            }else{
                return right_idx;
            }
        }

        /// @brief Sorts the element given by the index upwards and swaps it with its parent if it has a greater priority
        /// @param idx the indes to the element in the list
        /// @return the reference to the sorted element after the sort
        reference sort_up(size_type idx){

            // get its parent index
            size_type parent_idx = parent(idx);

            // iterate until top element is reached and as long as idx has a greater priority than its parent
            while((idx != 0) && this->_greater_priority(idx, parent_idx)){
                // switch if it has a greater priority than the parend
                std::swap(this->_buffer[idx], this->_buffer[parent_idx]);

                // assign new indices
                idx = parent_idx;
                parent_idx = parent(idx);
            }
            return this->_buffer.at(idx);
        }


        
    }
} // namespace embed

