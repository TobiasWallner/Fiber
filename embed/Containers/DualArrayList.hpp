#pragma once

// std
#include <cstddef>
#include <utility>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <ranges>
#include <functional>
#include <concepts> 

// embed
#include <embed/Containers/ArrayList.hpp>

// TODO: use the memcpy - to potentially use dma copy
// TODO: Make a ArrayListSlice that stores pointers and feels like woring with references to the elements of another list.
// TODO: Make a reference type (ArrayListRef) that references the whole list with a pointer to the data, a pointer to its size, an int of the capacity
// TODO: also make a const reference version of both slice and ref

namespace embed
{

    /**
     * @brief Two ArrayLists that share the same memory buffer
     * 
     * Two ArrayLists that internally grow in two different directions from opposite ends of the buffer.
     * Internally the left array list grows to the right and the right array list grows to the back.
     * ```
     * +---------------------+---------+----------------------+
     * | left array list --> | reserve | <-- right array list |
     * +---------------------+---------+----------------------+
     * ```
     * However, both array lists look like normal array lists. Both emplace back in the growing direction.
     * The left has normal pointer like iterators, and the right uses internally reverse iterators. 
     * 
     * @tparam T the value types of the container
     * @tparam N the maximum size of the container
     */
    template<class T, std::size_t N>
    class DualArrayList{
    public:
        using value_type = T;
        using size_type = std::size_t;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;

        using left_iterator = T*;
        using left_const_iterator = const T*;

        using right_iterator = std::reverse_iterator<left_iterator>;
        using right_const_iterator = std::reverse_iterator<left_const_iterator>; 

        using left_range = std::ranges::subrange<left_iterator>;
        using left_const_range = std::ranges::subrange<left_const_iterator>;

        using right_range = std::ranges::subrange<right_iterator>;
        using right_const_range = std::ranges::subrange<right_const_iterator>;

    private:
        alignas(T) std::byte _buffer[N * sizeof(T)];
        size_type _left_size = 0;
        size_type _right_size = 0;

    public:

        /// @brief default constructs two empty lists
        DualArrayList() = default;

        /// @brief Copy constructs both lists like from the other dual list
        template<size_t N1>
        DualArrayList(const DualArrayList<T, N1>& other){this->assign(other);}

        /// @brief Copy assigns both lists like from the other dual list
        template<size_t N1>
        DualArrayList& operator=(const DualArrayList<T, N1>& other){this->assign(other);}

        /// @brief Returns the size of the left list 
        constexpr size_type left_size() const {return this->_left_size;}

        /// @brief  Returns the size of the right list
        constexpr size_type right_size() const {return this->_right_size;}

        /// @brief Returns the combined size of both lists 
        constexpr size_type size() const {return this->left_size() + this->right_size();}

        /// @brief Returns the maximal size - is the same for left and right 
        constexpr size_type max_size() const {return N;}
        
        /// @brief Returns the capacity of the left list
        constexpr size_type left_capacity() const {return this->max_size() - this->right_size();}

        /// @brief Returns the capacity of the right list 
        constexpr size_type right_capacity() const {return this->max_size() - this->left_size();}

        /// @brief Returns the size of the reserve (aka.) number of free elements that has not been claimed by either list 
        constexpr size_type reserve() const {return this->max_size() - this->size();}

        /// @brief Returns `true` if the left list is empty 
        constexpr bool left_empty() const {return this->left_size() == 0;}

        /// @brief returns `true` if the right list is empty
        constexpr bool right_empty() const {return this->right_size() == 0;}

        /// @brief returns `true` if both lists are empty 
        constexpr bool empty() const {return this->size() == 0;}

        /// @brief returns `true` if the lists are full, all elements have been taken by either side, no more elements can be emplaced. 
        constexpr bool full() const {return this->size() == this->max_size();}

        constexpr pointer data(){return reinterpret_cast<pointer>(&this->_buffer[0]);}
        constexpr const_pointer data() const {return reinterpret_cast<const_pointer>(&this->_buffer[0]);}
        constexpr const_pointer cdata() const {return reinterpret_cast<const_pointer>(&this->_buffer[0]);}

        /// @brief returns an iterator to the beginning of the left list
        constexpr left_iterator left_begin(){return reinterpret_cast<T*>(&_buffer[0]);}

        /// @overload
        constexpr left_const_iterator left_begin() const {return reinterpret_cast<const T*>(&_buffer[0]);}

        /// @overload
        constexpr left_const_iterator left_cbegin() const {return reinterpret_cast<const T*>(&_buffer[0]);}

        /// @brief returns an iterator ot the beginning of the right list
        constexpr right_iterator right_begin(){return std::reverse_iterator(reinterpret_cast<T*>(&_buffer[0]) + N);}

        /// @overload 
        constexpr right_const_iterator right_begin() const {return std::reverse_iterator(reinterpret_cast<const T*>(&_buffer[0]) + N);}

        /// @overload
        constexpr right_const_iterator right_cbegin() const {return std::reverse_iterator(reinterpret_cast<const T*>(&_buffer[0]) + N);}

        /// @brief returns an iterator past the end of the left list
        constexpr left_iterator left_end(){return this->left_begin() + this->left_size();}

        /// @overload 
        constexpr left_const_iterator left_end() const {return this->left_begin() + this->left_size();}

        /// @overload
        constexpr left_const_iterator left_cend() const {return this->left_begin() + this->left_size();}

        /// @brief returns an iterator past the end of the right list 
        constexpr right_iterator right_end(){return this->right_begin() + this->right_size();}

        /// @overload 
        constexpr right_const_iterator right_end() const {return this->right_begin() + this->right_size();}

        /// @overload 
        constexpr right_const_iterator right_cend() const {return this->right_begin() + this->right_size();}

        /// @brief returns the left list as a subrange
        constexpr left_range left_subrange(){return left_range(this->left_begin(), this->left_end());}

        /// @overload 
        constexpr left_const_range left_subrange() const {return left_const_range(this->left_begin(), this->left_end());}
        
        /// @overload
        constexpr left_const_range left_csubrange() const {return left_const_range(this->left_begin(), this->left_end());}

        /// @brief returns the right list as a subrange
        constexpr right_range right_subrange(){return right_range(this->right_begin(), this->right_end());}

        /// @overload
        constexpr right_const_range right_subrange() const {return right_const_range(this->right_begin(), this->right_end());}

        /// @overload
        constexpr right_const_range right_csubrange() const {return right_const_range(this->right_begin(), this->right_end());}
    
        /// @brief Returns a reference of the first element in the left list
        /// @throws If the assertion level is at least `O1` throws an assertion failure if the list is empty 
        constexpr reference left_front(){
            EMBED_ASSERT_O1(!this->left_empty());
            return *this->left_begin();
        }
    
        /// @overload
        constexpr const_reference left_front() const {
            EMBED_ASSERT_O1(!this->left_empty());
            return *this->left_cbegin();
        }

        /// @brief Returns a reference of the last element in the left list
        /// @throws If the assertion level is at least `O1` throws an assertion failure if the list is empty 
        constexpr reference left_back(){
            EMBED_ASSERT_O1(!this->left_empty());
            return *(this->left_end()-1);
        }
    
        /// @overload
        constexpr const_reference left_back() const {
            EMBED_ASSERT_O1(!this->left_empty());
            return *(this->left_cend()-1);
        }
        
        /// @brief Returns a reference of the first element in the right list
        /// @throws If the assertion level is at least `O1` throws an assertion failure if the list is empty 
        constexpr reference right_front(){
            EMBED_ASSERT_O1(!this->right_empty());
            return *this->right_begin();
        }

        /// @overload
        constexpr const_reference right_front() const {
            EMBED_ASSERT_O1(!this->right_empty());
            return *this->right_cbegin();
        }

        /// @brief Returns a reference of the last element in the right list
        /// @throws If the assertion level is at least `O1` throws an assertion failure if the list is empty 
        constexpr reference right_back(){
            EMBED_ASSERT_O1(!this->right_empty());
            return *(this->right_end()-1);
        }
    
        /// @overload
        constexpr const_reference right_back() const {
            EMBED_ASSERT_O1(!this->right_empty());
            return *(this->right_cend()-1);
        }

        /// @brief Returns a reference to the element in the left list at the position with the given index
        /// @tparam UInt An unsigned integer/integral type
        /// @param i the index of the element to retreive
        /// @return a reference to the element at position i
        /// @throws If the assertion level is at least `O1` throws an assertion failure on out of bounds access
        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr reference left_at(const UInt i){
            EMBED_ASSERT_O1(i < this->left_size());    
            return *(this->left_begin()+i);
        }

        /// @overload 
        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr const_reference left_at(const UInt i) const {
            EMBED_ASSERT_O1(i < this->left_size());  
            return *(this->left_begin()+i);
        }

        /// @brief Returns a reference to the element in the right list at the position with the given index
        /// @tparam UInt An unsigned integer/integral type
        /// @param i the index of the element to retreive
        /// @return a reference to the element at position i
        /// @throws If the assertion level is at least `O1` throws an assertion failure on out of bounds access
        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr reference right_at(const UInt i){
            EMBED_ASSERT_O1(i < this->right_size());    
            return *(this->right_begin()+i);
        }

        /// @overload 
        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr const_reference right_at(const UInt i) const {
            EMBED_ASSERT_O1(i < this->right_size());  
            return *(this->right_begin()+i);
        }

        /// @brief Returns a reference to the element in the left list at the wrapped position with the given index
        /// @tparam SInt An unsigned integer/integral type
        /// @param si the index of the element to retreive, negative indices will wrap to the end of the list
        /// @return a reference to the element at position `si` or `left_size() - si` depending on signdines
        /// @throws If the assertion level is at least `O1` throws an assertion failure on out of bounds access
        template<class SInt> requires std::is_signed_v<SInt>
        constexpr reference left_at(SInt si){
            EMBED_ASSERT_O1(si < this->left_size());
            EMBED_ASSERT_O1(-si <= this->left_size());
            return *(((si >= 0) ? this->left_begin() : this->left_end()) + si);
        }

        /// @overload 
        template<class SInt> requires std::is_signed_v<SInt>
        constexpr const_reference left_at(const SInt si) const {
            EMBED_ASSERT_O1(si < this->left_size());
            EMBED_ASSERT_O1(-si <= this->left_size());
            return *(((si >= 0) ? this->left_begin() : this->left_end()) + si);
        }

        /// @brief Returns a reference to the element in the right list at the wrapped position with the given index
        /// @tparam SInt An unsigned integer/integral type
        /// @param si the index of the element to retreive, negative indices will wrap to the end of the list
        /// @return a reference to the element at position `si` or `right_size() - si` depending on signdines
        /// @throws If the assertion level is at least `O1` throws an assertion failure on out of bounds access
        template<class SInt> requires std::is_signed_v<SInt>
        constexpr reference right_at(SInt si){
            EMBED_ASSERT_O1(si < this->right_size());
            EMBED_ASSERT_O1(-si <= this->right_size());
            return *(((si >= 0) ? this->right_begin() : this->right_end()) + si);
        }

        /// @overload 
        template<class SInt> requires std::is_signed_v<SInt>
        constexpr const_reference right_at(const SInt si) const {
            EMBED_ASSERT_O1(si < this->right_size());
            EMBED_ASSERT_O1(-si <= this->right_size());
            return *(((si >= 0) ? this->right_begin() : this->right_end()) + si);
        }

        /// @brief Masked indexing of the left list
        /// @param mask the mask that selects which elements to get. `true` will be included, `false` excluded
        /// @return A ArrayList that contains all values where of this where mask is `true`
        ArrayList<T, N> left_at(const ArrayList<bool, N>& mask) const {
            ArrayList<T, N> result;
            auto thisItr = this->left_begin();
            const auto thisEnd = this->left_end();
            auto maskItr = mask.left_begin();
            const auto maskEnd = mask.left_end();
            for(; thisItr < thisEnd && maskItr < maskEnd; ++thisItr, (void)++maskItr){
                if(*maskItr){
                    result.left_emplace_back(*thisItr);
                }
            }
            return result;
        }

        /// @brief Masked indexing of the right list
        /// @param mask the mask that selects which elements to get. `true` will be included, `false` excluded
        /// @return A ArrayList that contains all values where of this where mask is `true`
        ArrayList<T, N> right_at(const ArrayList<bool, N>& mask) const {
            ArrayList<T, N> result;
            auto thisItr = this->right_begin();
            const auto thisEnd = this->right_end();
            auto maskItr = mask.right_begin();
            const auto maskEnd = mask.right_end();
            for(; thisItr < thisEnd && maskItr < maskEnd; ++thisItr, (void)++maskItr){
                if(*maskItr){
                    result.right_emplace_back(*thisItr);
                }
            }
            return result;
        }

        /// @brief constructs a list of from the left list at the provided list of indices
        /// @tparam Int a generic integer
        /// @param indices a list of indices that should be extracted
        /// @return a ArrayList containing all the elements from this that are contained in the `indices`
        template<class Int>
        requires std::is_integral_v<Int>
        ArrayList<T, N> left_at(const ArrayList<Int, N>& indices) const {
            ArrayList<T, N> result;
            for(const Int& index : indices){
                result.left_emplace_back(this->left_at(index));
            }
            return result;
        }

        /// @brief constructs a list of from the right list at the provided list of indices
        /// @tparam Int a generic integer
        /// @param indices a list of indices that should be extracted
        /// @return a ArrayList containing all the elements from this that are contained in the `indices`
        template<class Int>
        requires std::is_integral_v<Int>
        ArrayList<T, N> right_at(const ArrayList<Int, N>& indices) const {
            ArrayList<T, N> result;
            for(const Int& index : indices){
                result.right_emplace_back(this->right_at(index));
            }
            return result;
        }

        /// @brief emplaces (aka. pushes) an element to the back of the left list
        /// @details Actually constructs an element in place
        /// @tparam ...Args list of parameters that correspond to a constructor of the value type of the list
        /// @param ...args list of arguments to construct a value of the list
        /// @return a reference to the constructed list element
        template<class... Args>
        reference left_emplace_back(Args&&... args){
            EMBED_ASSERT_O1(this->full());
            pointer construct_at_addr = &*(this->left_begin() + this->left_size());
            new (construct_at_addr) T(std::forward<Args>(args)...);
            this->_left_size += 1;
            return this->left_back();
        }

        /// @brief emplaces (aka. pushes) an element to the back of the right list
        /// @details Actually constructs an element in place
        /// @tparam ...Args list of parameters that correspond to a constructor of the value type of the list
        /// @param ...args list of arguments to construct a value of the list
        /// @return a reference to the constructed list element
        template<class... Args>
        reference right_emplace_back(Args&&... args){
            EMBED_ASSERT_O1(this->full());
            pointer construct_at_addr = &*(this->right_begin() + this->right_size());
            new (construct_at_addr) T(std::forward<Args>(args)...);
            this->_right_size += 1;
            return this->right_back();
        }

        /// @brief clears the left list
        /// @details destructs all members if not trivially destructible and sets the size to zero
        void left_clear(){
            if constexpr (!std::is_trivially_destructible<T>::value){
                for(reference elem : this->left_subrange()){
                    elem->~T();
                }
            }
            this->_left_size = 0;
        }

        /// @brief clears the right list
        /// @details destructs all members if not trivially destructible and sets the size to zero
        void right_clear(){
            if constexpr (!std::is_trivially_destructible<T>::value){
                for(reference elem : this->right_subrange()){
                    elem->~T();
                }
            }
            this->_right_size = 0;
        }

        /// @brief clears both lists
        void clear(){
            this->left_clear();
            this->right_clear();
        }

        /// @brief appends `value` `count` many times to the left list
        /// @param count how often `value` should be `left_emplaced_back()`
        /// @param value the value to be created
        template<std::convertible_to<T> Ta>
        inline void left_append(const size_type count, const Ta& value){
            for(size_type i = 0; i < count; ++i) this->left_emplace_back(value);
        }

        /// @brief appends `value` `count` many times to the right list
        /// @param count how often `value` should be `right_emplaced_back()`
        /// @param value the value to be created
        template<std::convertible_to<T> Ta>
        inline void right_append(const size_type count, const Ta& value){
            for(size_type i = 0; i < count; ++i) this->right_emplace_back(value);
        }

        /// @brief assigns a value count many times to the left list.
        template<std::convertible_to<T> Ta>
        inline void left_assign(const size_type count, const Ta& value){
            this->left_clear();
            this->left_append(count, value);
        }

        /// @brief assigns a value count many times to the right list.
        template<std::convertible_to<T> Ta>
        inline void right_assign(const size_type count, const Ta& value){
            this->right_clear();
            this->right_append(count, value);
        }

        /// @brief appends a range of elements defined by foreward iterators using the closed-open principle [first, last) 
        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        inline void left_append(Itr first, Itr last){
            for(; first != last; ++first) 
                this->left_emplace_back(*first);
        }

        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        inline void right_append(Itr first, Itr last){
            for(; first != last; ++first) 
                this->right_emplace_back(*first);
        }

        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        inline void left_assign(Itr first, Itr last){
            this->left_clear();
            this->left_append(first, last);
        }

        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        inline void right_assign(Itr first, Itr last){
            this->right_clear();
            this->right_append(first, last);
        }

        template<std::convertible_to<T> Ta>
        inline void left_append(std::initializer_list<Ta> ilist){
            this->left_append(ilist.begin(), ilist.end());
        }

        template<std::convertible_to<T> Ta>
        inline void right_append(std::initializer_list<Ta> ilist){
            this->right_append(ilist.begin(), ilist.end());
        }

        template<std::convertible_to<T> Ta>
        inline void left_assign(std::initializer_list<Ta> ilist){
            this->left_clear();
            this->left_append(ilist);
        }

        template<std::convertible_to<T> Ta>
        inline void right_assign(std::initializer_list<Ta> ilist){
            this->right_clear();
            this->right_append(ilist);
        }

        template<std::ranges::forward_range Range>
        inline void left_append(const Range& range){
            this->left_append(range.begin(), range.end());
        }

        template<std::ranges::forward_range Range>
        inline void right_append(const Range& range){
            this->right_append(range.begin(), range.end());
        }

        template<std::ranges::forward_range Range>
        inline void left_assign(const Range& range){
            this->left_clear();
            this->left_append(range);
        }

        template<std::ranges::forward_range Range>
        inline void right_assign(const Range& range){
            this->right_clear();
            this->right_append(range);
        }

        template<size_t N1>
        inline void assign(const DualArrayList<T, N1>& other){
            this->left_assign(other.left_subrange());
            this->right_assign(other.right_subrange());
        }

        constexpr size_type left_to_index(const left_const_iterator pos) const {
            EMBED_ASSERT_O1(this->left_begin() <= pos && pos < this->left_end());
            return pos - this->left_begin();
        }

        constexpr size_type right_to_index(const right_const_iterator pos) const {
            EMBED_ASSERT_O1(this->right_begin() <= pos && pos < this->right_end());
            return pos - this->right_begin();
        }

        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr left_iterator left_to_iterator(const UInt pos){
            EMBED_ASSERT_O1(pos < this->left_size());
            return this->left_begin() + pos;
        }

        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr right_iterator right_to_iterator(const UInt pos){
            EMBED_ASSERT_O1(pos < this->right_size());
            return this->right_begin() + pos;
        }

        template<class SInt> requires std::is_signed_v<SInt>
        constexpr left_iterator left_to_iterator(const SInt pos){
            EMBED_ASSERT_O1(pos < this->left_size());
            EMBED_ASSERT_O1(-pos <= this->left_size());
            return ((pos >= 0) ? this->left_begin() : this->left_end()) + pos;
        }

        template<class SInt> requires std::is_signed_v<SInt>
        constexpr right_iterator right_to_iterator(const SInt pos){
            EMBED_ASSERT_O1(pos < this->right_size());
            EMBED_ASSERT_O1(-pos <= this->right_size());
            return ((pos >= 0) ? this->right_begin() : this->right_end()) + pos;
        }

        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr left_const_iterator left_to_iterator(const UInt pos) const {
            EMBED_ASSERT_O1(pos < this->left_size());
            return this->left_begin() + pos;
        }

        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr right_const_iterator right_to_iterator(const UInt pos) const {
            EMBED_ASSERT_O1(pos < this->right_size());
            return this->right_begin() + pos;
        }

        template<class SInt> requires std::is_signed_v<SInt>
        constexpr left_const_iterator left_to_iterator(const SInt pos) const {
            EMBED_ASSERT_O1(pos < this->left_size());
            EMBED_ASSERT_O1(-pos <= this->left_size());
            return ((pos >= 0) ? this->left_begin() : this->left_end()) + pos;
        }

        template<class SInt> requires std::is_signed_v<SInt>
        constexpr right_const_iterator right_to_iterator(const SInt pos) const {
            EMBED_ASSERT_O1(pos < this->right_size());
            EMBED_ASSERT_O1(-pos <= this->right_size());
            return ((pos >= 0) ? this->right_begin() : this->right_end()) + pos;
        }

        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr left_const_iterator left_to_const_iterator(const UInt pos) const {
            EMBED_ASSERT_O1(pos < this->left_size());
            return this->left_begin() + pos;
        }

        template<class UInt> requires std::is_unsigned_v<UInt>
        constexpr right_const_iterator right_to_const_iterator(const UInt pos) const {
            EMBED_ASSERT_O1(pos < this->right_size());
            return this->right_begin() + pos;
        }

        template<class SInt> requires std::is_signed_v<SInt>
        constexpr left_const_iterator left_to_const_iterator(const SInt pos) const {
            EMBED_ASSERT_O1(pos < this->left_size());
            EMBED_ASSERT_O1(-pos <= this->left_size());
            return ((pos >= 0) ? this->left_begin() : this->left_end()) + pos;
        }

        template<class SInt> requires std::is_signed_v<SInt>
        constexpr right_const_iterator right_to_const_iterator(const SInt pos) const {
            EMBED_ASSERT_O1(pos < this->right_size());
            EMBED_ASSERT_O1(-pos <= this->right_size());
            return ((pos >= 0) ? this->right_begin() : this->right_end()) + pos;
        }

        constexpr left_iterator left_unconst(const left_const_iterator pos) {
            EMBED_ASSERT_O1(this->left_begin() <= pos);
            EMBED_ASSERT_O1(pos < this->left_end());
            return this->left_begin() + this->left_to_index(pos);
        }

        constexpr right_iterator right_unconst(const right_const_iterator pos) {
            EMBED_ASSERT_O1(this->right_begin() <= pos);
            EMBED_ASSERT_O1(pos < this->right_end());
            return this->right_begin() + this->right_to_index(pos);
        }

        template<std::convertible_to<T> Ta>
        left_iterator left_insert(const left_const_iterator pos, const Ta& value){
            EMBED_ASSERT_O1(this->full());
            for(auto i = this->left_end(); i != pos; --i) *i = std::move(*(i-1));
            this->_left_size += 1;
            left_iterator pos_ = left_unconst(pos);
            *pos_ = value;
            return pos_;
        }

        template<std::convertible_to<T> Ta>
        right_iterator right_insert(const right_const_iterator pos, const Ta& value){
            EMBED_ASSERT_O1(this->full());
            for(auto i = this->right_end(); i != pos; --i) *i = std::move(*(i-1));
            this->_right_size += 1;
            right_iterator pos_ = right_unconst(pos);
            *pos_ = value;
            return pos_;
        }

        template<std::convertible_to<T> Ta>
        left_iterator left_insert(const left_const_iterator pos, Ta&& value){
            EMBED_ASSERT_O1(this->full());
            for(auto i = this->left_end(); i != pos; --i) *i = std::move(*(i-1));
            this->_left_size += 1;
            left_iterator pos_ = left_unconst(pos);
            *pos_ = std::move(value);
            return pos_;
        }

        template<std::convertible_to<T> Ta>
        right_iterator right_insert(const right_const_iterator pos, Ta&& value){
            EMBED_ASSERT_O1(this->full());
            for(auto i = this->right_end(); i != pos; --i) *i = std::move(*(i-1));
            this->_right_size += 1;
            right_iterator pos_ = right_unconst(pos);
            *pos_ = std::move(value);
            return pos_;
        }

        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        left_iterator left_insert(const left_const_iterator pos, Itr first, Itr last){
            size_type dist = std::distance(first, last);
            EMBED_ASSERT_O1(dist > this->reserve());
            for(auto i = this->left_end(); i != pos; --i) *(i + dist - 1) = std::move(*(i - 1));
            left_iterator insertIterator = this->left_unconst(pos);
            for(; first != last; ++first, (void)++insertIterator) *insertIterator = *first;
            this->_left_size += dist;
            return left_unconst(pos);
        }

        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        right_iterator right_insert(const right_const_iterator pos, Itr first, Itr last){
            size_type dist = std::distance(first, last);
            EMBED_ASSERT_O1(dist > this->reserve());
            for(auto i = this->right_end(); i != pos; --i) *(i + dist - 1) = std::move(*(i - 1));
            right_iterator insertIterator = this->right_unconst(pos);
            for(; first != last; ++first, (void)++insertIterator) *insertIterator = *first;
            this->_right_size += dist;
            return right_unconst(pos);
        }

        template<std::ranges::forward_range Range>
        inline left_iterator left_insert(const left_const_iterator pos, const Range& range){
            return this->left_insert(pos, range.begin(), range.end());
        }

        template<std::ranges::forward_range Range>
        inline right_iterator right_insert(const right_const_iterator pos, const Range& range){
            return this->right_insert(pos, range.begin(), range.end());
        }

        template<std::convertible_to<T> Ti>
        inline left_iterator left_insert(const left_const_iterator pos, std::initializer_list<Ti> ilist){
            return this->left_insert(pos, ilist.begin(), ilist.end());
        }

        template<std::convertible_to<T> Ti>
        inline right_iterator right_insert(const right_const_iterator pos, std::initializer_list<Ti> ilist){
            return this->right_insert(pos, ilist.begin(), ilist.end());
        }

        /**
         * @brief Inserts a value into the left list at the index position
         * 
         * If the integer type is signed, it will check wether or not the index is negative.
         * If the integer is negative the value will wrap around the container
         * 
         * > Note: positive values will insert at/before the value the index points to, 
         * > while negative values will insert after if you view the range only in positive indices.
         * > So an insertion at `-1` is equivalent to an insertion at the `end()` iterator
         */
        template<std::integral Int>
        inline left_iterator left_insert(const Int index, const T& value){
            if constexpr (std::is_unsigned_v<Int>){
                return this->left_insert(this->left_begin() + index, value);
            }else{
                if(index >= 0){
                    return this->left_insert(this->left_begin() + index, value);
                }else{
                    return this->left_insert(this->left_end() + (index + 1), value);
                }
            }
        }

        /**
         * @brief Inserts a value into the right list at the index position
         * 
         * If the integer type is signed, it will check wether or not the index is negative.
         * If the integer is negative the value will wrap around the container
         * 
         * > Note: positive values will insert at/before the value the index points to, 
         * > while negative values will insert after if you view the range only in positive indices.
         * > So an insertion at `-1` is equivalent to an insertion at the `end()` iterator
         */
        template<std::integral Int>
        inline right_iterator right_insert(const Int index, const T& value){
            if constexpr (std::is_unsigned_v<Int>){
                return this->right_insert(this->right_begin() + index, value);
            }else{
                if(index >= 0){
                    return this->right_insert(this->right_begin() + index, value);
                }else{
                    return this->right_insert(this->right_end() + (index + 1), value);
                }
            }
        }

        /// @overload 
        template<std::integral Int>
        inline left_iterator left_insert(const Int index, T&& value){
            if constexpr (std::is_unsigned_v<Int>){
                return this->left_insert(this->left_begin() + index, std::move(value));
            }else{
                if(index >= 0){
                    return this->left_insert(this->left_begin() + index, std::move(value));
                }else{
                    return this->left_insert(this->left_end() + (index + 1), std::move(value));
                }
            }
        }

        /// @overload
        template<std::integral Int>
        inline right_iterator right_insert(const Int index, T&& value){
            if constexpr (std::is_unsigned_v<Int>){
                return this->right_insert(this->right_begin() + index, std::move(value));
            }else{
                if(index >= 0){
                    return this->right_insert(this->right_begin() + index, std::move(value));
                }else{
                    return this->right_insert(this->right_end() + (index + 1), std::move(value));
                }
            }
        }

        template<std::integral Int, std::ranges::forward_range Range>
        inline left_iterator left_insert(const Int index, const Range& range){
            if constexpr (std::is_unsigned_v<Int>){
                return this->left_insert(this->left_begin() + index, range);
            }else{
                if(index >= 0){
                    return this->left_insert(this->left_begin() + index, range);
                }else{
                    return this->left_insert(this->left_end() + (index + 1), range);
                }
            }
        }

        template<std::integral Int, std::ranges::forward_range Range>
        inline right_iterator right_insert(const Int index, const Range& range){
            if constexpr (std::is_unsigned_v<Int>){
                return this->right_insert(this->right_begin() + index, range);
            }else{
                if(index >= 0){
                    return this->right_insert(this->right_begin() + index, range);
                }else{
                    return this->right_insert(this->right_end() + (index + 1), range);
                }
            }
        }

        template<std::integral Int, std::forward_iterator Itr>
        inline left_iterator left_insert(const Int index, Itr first, Itr last){
            if constexpr (std::is_unsigned_v<Int>){
                return this->left_insert(this->left_begin() + index, first, last);
            }else{
                if(index >= 0){
                    return this->left_insert(this->left_begin() + index, first, last);
                }else{
                    return this->left_insert(this->left_end() + (index + 1), first, last);
                }
            }
        }

        template<std::integral Int, std::forward_iterator Itr>
        inline right_iterator right_insert(const Int index, Itr first, Itr last){
            if constexpr (std::is_unsigned_v<Int>){
                return this->right_insert(this->right_begin() + index, first, last);
            }else{
                if(index >= 0){
                    return this->right_insert(this->right_begin() + index, first, last);
                }else{
                    return this->right_insert(this->right_end() + (index + 1), first, last);
                }
            }
        }

        template<std::integral Int, std::convertible_to<T> Ti>
        inline left_iterator left_insert(const Int index, std::initializer_list<Ti> ilist){
            if constexpr (std::is_unsigned_v<Int>){
                return this->left_insert(this->left_begin() + index, ilist);
            }else{
                if(index >= 0){
                    return this->left_insert(this->left_begin() + index, ilist);
                }else{
                    return this->left_insert(this->left_end() + (index + 1), ilist);
                }
            }
        }

        template<std::integral Int, std::convertible_to<T> Ti>
        inline right_iterator right_insert(const right_const_iterator index, std::initializer_list<Ti> ilist){
            if constexpr (std::is_unsigned_v<Int>){
                return this->right_insert(this->right_begin() + index, ilist);
            }else{
                if(index >= 0){
                    return this->right_insert(this->right_begin() + index, ilist);
                }else{
                    return this->right_insert(this->right_end() + (index + 1), ilist);
                }
            }
        }

        left_iterator left_erase(left_const_iterator cpos){
            EMBED_ASSERT_O1(!this->left_empty());
            left_iterator destItr = this->left_unconst(cpos);
            left_iterator sourceItr = destItr + 1;
            for(; sourceItr < this->left_end(); ++sourceItr, (void)++destItr) *destItr = std::move(*sourceItr);
            if constexpr (!std::is_trivially_destructible<T>::value){
                this->left_back().~T();
            }
            this->_left_size -= 1;
            return this->left_unconst(cpos);
        }

        right_iterator right_erase(right_const_iterator cpos){
            EMBED_ASSERT_O1(!this->right_empty());
            right_iterator destItr = this->right_unconst(cpos);
            right_iterator sourceItr = destItr + 1;
            for(; sourceItr < this->right_end(); ++sourceItr, (void)++destItr) *destItr = std::move(*sourceItr);
            if constexpr (!std::is_trivially_destructible<T>::value){
                this->right_back().~T();
            }
            this->_right_size -= 1;
            return this->right_unconst(cpos);
        }

        template<std::integral Int>
        inline left_iterator left_erase(const Int pos){
            return this->left_erase(this->left_to_iterator(pos));
        }

        template<std::integral Int>
        inline right_iterator right_erase(const Int pos){
            return this->right_erase(this->right_to_iterator(pos));
        }

        left_iterator left_erase(left_const_iterator first, left_const_iterator last){
            EMBED_ASSERT_O1(this->left_begin() <= first);
            EMBED_ASSERT_O1(first < this->left_end());
            EMBED_ASSERT_O1(this->left_begin() <= last);
            EMBED_ASSERT_O1(last < this->left_end());
            left_iterator destItr = this->left_unconst(first);
            left_iterator sourceItr = this->left_unconst(last);
            for(; (destItr != last) && (sourceItr != this->left_end()); ++destItr, (void)++sourceItr){
                *destItr = std::move(*sourceItr);
            }
            
            if constexpr (!std::is_trivially_destructible<T>::value){
                for(; destItr != this->left_end(); ++destItr){
                    destItr->~T();
                }
            }
            const auto dist = std::distance(first, last);
            this->_left_size -= dist;
            return this->left_unconst(last-dist);
        }

        right_iterator right_erase(right_const_iterator first, right_const_iterator last){
            EMBED_ASSERT_O1(this->right_begin() <= first);
            EMBED_ASSERT_O1(first < this->right_end());
            EMBED_ASSERT_O1(this->right_begin() <= last);
            EMBED_ASSERT_O1(last < this->right_end());
            right_iterator destItr = this->right_unconst(first);
            right_iterator sourceItr = this->right_unconst(last);
            for(; (destItr != last) && (sourceItr != this->right_end()); ++destItr, (void)++sourceItr){
                *destItr = std::move(*sourceItr);
            }
            
            if constexpr (!std::is_trivially_destructible<T>::value){
                for(; destItr != this->right_end(); ++destItr){
                    destItr->~T();
                }
            }

            const auto dist = std::distance(first, last);
            this->_right_size -= dist;
            return this->right_unconst(last-dist);
        }

        template<class Callable>
        std::size_t left_erase_if(Callable&& f){
            left_iterator read_itr = this->left_begin();
            const left_iterator end_itr = this->left_end();

            // search first that is true
            for(;read_itr != end_itr; ++read_itr){
                if(f(*read_itr)) break;
            }

            // return if it did not aplie to any
            if(read_itr == end_itr) return 0;
            
            // setup first read and write iterators
            left_iterator write_itr = read_itr;
            ++read_itr;

            // move down or skip if erased
            while(read_itr != end_itr){
                if(f(*read_itr)){
                    ++read_itr;
                }else{
                    *write_itr = std::move(*read_itr);
                    ++write_itr;
                    ++read_itr;
                }
            }
            // calculate new and erased size
            std::size_t new_size = write_itr - this->left_begin();
            std::size_t n_erased = this->left_end() - write_itr;

            // destroy remaining
            if constexpr (!std::is_trivially_destructible<T>::value){
                for(; write_itr != end_itr; ++write_itr){
                    write_itr->~T();
               }
            }
            this->_left_size = new_size;
            return n_erased;
        }

        template<class Callable>
        std::size_t right_erase_if(Callable&& f){
            right_iterator read_itr = this->right_begin();
            const right_iterator end_itr = this->right_end();

            // search first that is true
            for(;read_itr != end_itr; ++read_itr){
                if(f(*read_itr)) break;
            }

            // return if it did not aplie to any
            if(read_itr == end_itr) return 0;
            
            // setup first read and write iterators
            right_iterator write_itr = read_itr;
            ++read_itr;

            // move down or skip if erased
            while(read_itr != end_itr){
                if(f(*read_itr)){
                    ++read_itr;
                }else{
                    *write_itr = std::move(*read_itr);
                    ++write_itr;
                    ++read_itr;
                }
            }
            // calculate new and erased size
            std::size_t new_size = std::distance(this->right_begin(), write_itr);
            std::size_t n_erased = std::distance(write_itr, this->right_end());

            // destroy remaining
            if constexpr (!std::is_trivially_destructible<T>::value){
                for(; write_itr != end_itr; ++write_itr){
                    write_itr->~T();
               }
            }
            this->_right_size = new_size;
            return n_erased;
        }

        template<class Int> requires std::is_integral_v<Int>
        left_iterator left_erase(Int first, Int last){
            return this->left_erase(this->left_to_iterator(first), this->left_to_iterator(last));
        }

        template<class Int> requires std::is_integral_v<Int>
        right_iterator right_erase(Int first, Int last){
            return this->right_erase(this->right_to_iterator(first), this->right_to_iterator(last));
        }

        void left_pop_back(){
            EMBED_ASSERT_O1(!this->left_empty());
            if constexpr (!std::is_trivially_destructible<T>::value){
                this->left_back().~T();
            }
            this->_left_size -= 1;
        }

        void right_pop_back(){
            EMBED_ASSERT_O1(!this->right_empty());
            if constexpr (!std::is_trivially_destructible<T>::value){
                this->right_back().~T();
            }
            this->_right_size -= 1;
        }
        
        template<class Function>
        constexpr void left_for_each(Function&& function){
            for(left_iterator itr = this->left_begin(); itr != this->left_end(); ++itr){
                *itr = function(*itr);
            }
        }

        template<class Function>
        constexpr void right_for_each(Function&& function){
            for(right_iterator itr = this->right_begin(); itr != this->right_end(); ++itr){
                *itr = function(*itr);
            }
        }
    };

    template<class T, std::size_t N>
    embed::OStream& operator << (OStream& stream, const DualArrayList<T, N>& dlist){
        stream << "embed::DualArrayList<" << typeid(T).name() << ", " << N << ">{\n";
        stream << "  left: [";

        {
            char seperator[] = {'\0', ' ', '\0'};
            for(auto& elem : dlist.left_subrange()){
                stream << seperator << elem;
                seperator[0] = ',';
            }
        }

        stream << "]\n  right: [";

        {
            char seperator[] = {'\0', ' ', '\0'};
            for(auto& elem : dlist.right_subrange()){
                stream << seperator << elem;
                seperator[0] = ',';
            }
        }

        stream << "]\n}\n";
        return stream;
    }

    /**
     * @brief A reference to the left side of a DualArrayList
     * @see DualArrayList
     */
    template<class T, std::size_t N>
    class LeftDualArrayList{
    public:
        using value_type = DualArrayList<T, N>::value_type;
        using size_type = DualArrayList<T, N>::size_type;
        using reference = DualArrayList<T, N>::reference;
        using const_reference = DualArrayList<T, N>::const_reference;
        using iterator = DualArrayList<T, N>::left_iterator;
        using const_iterator = DualArrayList<T, N>::left_const_iterator;
        using pointer = DualArrayList<T, N>::pointer;
        using const_pointer = DualArrayList<T, N>::const_pointer;

    private:
        DualArrayList<T, N>& _list;

    public:
        
        LeftDualArrayList(DualArrayList<T, N>& list) : _list(list){}

        /// @brief returns the size/count of live elements in the container 
        constexpr size_type size() const {return this->_list.left_size();}

        /// @brief returns the capacity of the container. Since this is a statically allocated container this is also the maximal size.
        constexpr size_type capacity() const {return this->_list.left_capacity();}

        /// @brief returns the maximal number of elements that can be stored in the container 
        constexpr size_type max_size() const {return this->_list.max_size();}

        /// @brief returns the reserve - number of elements that can be stored until the container is full 
        constexpr size_type reserve() const {return this->_list.reserve();}

        /// @brief returns true if there are not elements in the container, aka. the container is empty. 
        constexpr bool empty() const {return this->_list.left_empty();}

        /// @brief returns true if the container is full and no more elements can be stored in the container 
        constexpr bool full() const {return this->_list.full();}

        /// @brief returns an iterator to the start 
        constexpr iterator begin() {return this->_list.left_begin();}

        /// @brief returns a const-iterator to the start 
        constexpr const_iterator begin() const {return this->_list.left_begin();}

        /// @brief returns a const-iterator to the start 
        constexpr const_iterator cbegin() const {return this->_list.left_cbegin();}

        /// @brief returns an iterator past the end 
        constexpr iterator end() {return this->_list.left_end();}

        /// @brief returns a const-iterator past the end
        constexpr const_iterator end() const {return this->_list.left_end();}

        /// @brief returns a const-iterator past the end
        constexpr const_iterator cend() const {return this->_list.left_cend();}

        /// @brief returns a reference to the first element in the buffer 
        constexpr T& front() {return this->_list.left_front();}

        /// @brief returns a const-reference to the first element int the buffer
        constexpr const T& front() const {return this->_list.left_front();}

        /// @brief returns a reference to the last element in the buffer 
        constexpr T& back() {return this->_list.left_back();}

        /// @brief returns a const reference to the last element in the buffer 
        constexpr const T& back() const {return this->_list.left_back();}

        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr T& at(const Int i){return this->_list.left_at(i);}

        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr const T& at(const Int i) const {return this->_list.left_at(i);}


        /// @brief Masked indexing
        /// @param mask the mask that selects which elements to get. `true` will be included, `false` excluded
        /// @return A ArrayList that contains all values where of this where mask is `true`
        ArrayList<T, N> at(const ArrayList<bool, N>& mask) const {return this->_list.left_at(mask);}

        /// @brief Inices list indexing
        /// @tparam Int a generic integer
        /// @param indices a list of indices that should be extracted
        /// @return a ArrayList containing all the elements from this that are contained in the `indices`
        template<std::integral Int>
        ArrayList<T, N> at(const ArrayList<Int, N>& indices) const {return this->_list.left_at(indices);}

        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr T& operator[](const Int i){return this->left_at(i);}
        
        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr const T& operator[](const Int i) const {return this->left_at(i);}

        /// @brief accesses all elements where `mask` is `true` 
        ArrayList<T, N> operator[](const ArrayList<bool, N>& mask) const {return this->left_at(mask);}

        /// @brief accesses all elements at the given `indices` 
        template<std::integral Int>
        ArrayList<T, N> operator[](const ArrayList<Int, N>& indices) const {return this->left_at(indices);}

        /// @brief emplaces (aka. pushes) an element to the back of the list
        /// @details Actually constructs an element in place
        /// @tparam ...Args list of parameters that correspond to a constructor of the value type of the list
        /// @param ...args list of arguments to construct a value of the list
        /// @return a reference to the constructed list element
        template<class... Args>
        T& emplace_back(Args&&... args){return this->_list.left_emplace_back(std::forward<Args>(args)...);}

        /// @brief clears the list - destructs all members if necessary and sets the size to zero
        void clear(){this->_list.left_clear();}

        /// @brief appends `value` `count` many times
        /// @param count how often `value` should be emplaced_back()
        /// @param value the value to be created
        inline void append(const size_type count, const T& value){this->_list.left_append(count, value);}

        /// @brief assigns a value to the list. after which value is the only element in the list
        inline void assign(const size_type count, const T& value){this->_list.left_assign(count, value);}

        /// @brief appends a range defined by foreward iterators using the closed-open principle [first, last)
        template<std::forward_iterator Itr>
        inline void append(Itr first, Itr last){this->_list.left_append(first, last);}

        /// @brief assigns a range defined by foreward iterators using the closed-open principle [first, last). After the assignment the list has the size of the assigned range
        template<std::forward_iterator Itr>
        inline void assign(Itr first, Itr last){this->_list.left_assign(first, last);}

        /// @brief appens an initilizer_list
        inline void append(std::initializer_list<T> ilist){this->_list.left_append(ilist);}

        /// @brief assigns an initializer_list
        inline void assign(std::initializer_list<T> ilist){this->_list.left_assign(ilist);}

        /// @brief Appends the content of a range
        template<std::ranges::forward_range Range>
        inline void append(const Range& range){this->_list.left_append(range);}

        /// @brief Assigns the content of a range
        template<std::ranges::forward_range Range>
        inline void assign(const Range& range){this->_list.left_assign(range);}

        /// @brief turns the passed position given by an iterator into an integer 
        constexpr size_type to_index(const const_iterator pos) const {return this->_list.left_to_index(pos);}

        /// @brief turns the passed unsigned integer into an iterator pointing to the same position 
        template<std::integral Int>
        constexpr iterator to_iterator(const Int pos){return this->_list.left_to_iterator(pos);}

        /// @brief turns the passed unsigned integer into a cosnt_iterator pointing to the same position 
        template<std::integral Int>
        constexpr const_iterator to_iterator(const Int pos) const {return this->_list.left_to_iterator(pos);}

        /// @brief turns the passed unsigned integer into a cosnt_iterator pointing to the same position 
        template<std::integral Int>
        constexpr const_iterator to_const_iterator(const Int pos) const {return this->_list.left_to_const_iterator(pos);}

        /// @brief removes the constnes of an iterator if the user has access to the mutable (un-const) container 
        constexpr iterator unconst(const const_iterator pos) {return this->_list.left_unconst(pos);}

        /// @brief Inserts a value
        /// @param pos the position at which the value should be inserted
        /// @param value the value that the element at that position should have after the insertion
        /// @return an iterator pointing to the inserted value
        iterator insert(const const_iterator pos, const T& value){return this->_list.left_insert(pos, value);}

        /// @brief Inserts a value
        /// @param pos the position at which the value should be inserted
        /// @param value the value that the element at that position should have after the insertion
        /// @return an iterator pointing to the inserted value
        iterator insert(const const_iterator pos, T&& value){return this->_list.left_insert(pos, std::move(value));}

        /// @brief inserts a range at a given position
        /// @tparam Itr a generic iterator that follows the concept std::forward_iterator
        /// @param pos the position at which should be inserted given by an iterator
        /// @param first the start of the range that should be inserted
        /// @param last the past the end iterator to which should be inserted
        /// @return an iterator to the start of the start of the insertion
        template<std::forward_iterator Itr>
        iterator insert(const const_iterator pos, Itr first, Itr last){return this->_list.left_insert(pos, first, last);}

        /// @brief inserts a range at the given position
        /// @tparam Range a generic range that follows the concept `std::ranges::forward_range`
        /// @param pos the insertion point
        /// @param range the range that should be inserted
        /// @return an iterator to the inserted range
        template<std::ranges::forward_range Range>
        inline iterator insert(const const_iterator pos, const Range& range){return this->_list.left_insert(pos, range);}

        inline iterator insert(const const_iterator pos, const std::initializer_list<T>& ilist){return this->_list.left_insert(pos, ilist);}

        /// @brief inserts the `value` at the `pos`ition passed as an integer that wraps if it is a signed type 
        template<std::integral Int>
        inline iterator insert(const Int pos, const T& value){return this->_list.left_insert(pos, value);}

        /// @brief inserts the `value` at the `pos`ition passed as an integer that wraps if it is a signed type
        template<std::integral Int>
        inline iterator insert(const Int pos, T&& value){return this->_list.left_insert(pos, std::move(value));}

        /// @brief inserts the `range` at the `pos`ition passed as an integer that wraps if it is a signed type
        template<std::integral Int, std::ranges::forward_range Range>
        inline iterator insert(const Int pos, const Range& range){return this->_list.left_insert(pos, range);}

        template<std::integral Int>
        inline iterator insert(const Int pos, const std::initializer_list<T>& ilist){return this->_list.left_insert(pos, ilist);}

        /// @brief inserts the closed-open [`first`, `last`) range at the given `pos`ition
        template<std::integral Int, std::forward_iterator Itr>
        inline iterator insert(const Int pos, Itr first, Itr last){return this->_list.left_insert(pos, first, last);}

        /// @brief erases/removes the element at the position pointed to by `cpos` 
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed one
        iterator erase(const_iterator cpos){return this->_list.left_erase(cpos);}

        /// @brief erases/removes the element at the position pointed to by `cpos` 
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed one
        template<std::integral Int>
        inline iterator erase(const Int pos){return this->_list.left_erase(pos);}

        /// @brief erases/removes the range given by the closed-open iterators [first, last)
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed ones
        iterator erase(const_iterator first, const_iterator last){return this->_list.left_erase(first, last);}

        /// @brief erases elements from the list if they satisfy the callable
        /// @tparam Callable Object that can be called like `bool f(const T&)` or `bool f(T)`.
        /// @param f Condition that returns `true` if that element should be erased from the list.
        template<class Callable>
        std::size_t erase_if(Callable&& f){return this->_list.left_erase_if(f);}

        /// @brief erases/removes the range given by the closed-open indices [first, last)
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed ones
        template<std::integral Int>
        iterator erase(Int first, Int last){return this->_list.left_erase(first, last);}

        /// @brief removes and destructs the last element
        void pop_back(){this->_list.left_pop_back();}

        /// @brief Applies the negation (!) operator to all elements and returns it as a bool list 
        ArrayList<bool, N> operator!() const {
            ArrayList<bool, N> result;
            for(const auto& elem : *this){
                result.emplace_back(!elem);
            }
            return result;
        }

        /// @brief Applies the function to each element of the list in-place
        /// @note This is an in-place operation and will change the list. If you want to create a new list with the transformed values, use `embed::for_each(const ArrayList<T, N>&, std::function<T, const T&> function)` instead
        /// @param function the function being applied to change/transform each element
        template<class Function>
        constexpr void for_each(Function&& function){this->_list.left_for_each(function);}

    };

    /**
     * @brief A reference to the right side of a DualArrayList
     * @see DualArrayList
     */
    template<class T, std::size_t N>
    class RightDualArrayList{
    public:
        using value_type = DualArrayList<T, N>::value_type;
        using size_type = DualArrayList<T, N>::size_type;
        using reference = DualArrayList<T, N>::reference;
        using const_reference = DualArrayList<T, N>::const_reference;
        using iterator = DualArrayList<T, N>::right_iterator;
        using const_iterator = DualArrayList<T, N>::right_const_iterator;
        using pointer = DualArrayList<T, N>::pointer;
        using const_pointer = DualArrayList<T, N>::const_pointer;

    private:
        DualArrayList<T, N>& _list;

    public:
        
        RightDualArrayList(DualArrayList<T, N>& list) : _list(list){}

        /// @brief returns the size/count of live elements in the container 
        constexpr size_type size() const {return this->_list.right_size();}

        /// @brief returns the capacity of the container. Since this is a statically allocated container this is also the maximal size.
        constexpr size_type capacity() const {return this->_list.right_capacity();}

        /// @brief returns the maximal number of elements that can be stored in the container 
        constexpr size_type max_size() const {return this->_list.max_size();}

        /// @brief returns the reserve - number of elements that can be stored until the container is full 
        constexpr size_type reserve() const {return this->_list.reserve();}

        /// @brief returns true if there are not elements in the container, aka. the container is empty. 
        constexpr bool empty() const {return this->_list.right_empty();}

        /// @brief returns true if the container is full and no more elements can be stored in the container 
        constexpr bool full() const {return this->_list.full();}

        /// @brief returns an iterator to the start 
        constexpr iterator begin() {return this->_list.right_begin();}

        /// @brief returns a const-iterator to the start 
        constexpr const_iterator begin() const {return this->_list.right_begin();}

        /// @brief returns a const-iterator to the start 
        constexpr const_iterator cbegin() const {return this->_list.right_cbegin();}

        /// @brief returns an iterator past the end 
        constexpr iterator end() {return this->_list.right_end();}

        /// @brief returns a const-iterator past the end
        constexpr const_iterator end() const {return this->_list.right_end();}

        /// @brief returns a const-iterator past the end
        constexpr const_iterator cend() const {return this->_list.right_cend();}

        /// @brief returns a reference to the first element in the buffer 
        constexpr T& front() {return this->_list.right_front();}

        /// @brief returns a const-reference to the first element int the buffer
        constexpr const T& front() const {return this->_list.right_front();}

        /// @brief returns a reference to the last element in the buffer 
        constexpr T& back() {return this->_list.right_back();}

        /// @brief returns a const reference to the last element in the buffer 
        constexpr const T& back() const {return this->_list.right_back();}

        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr T& at(const Int i){return this->_list.right_at(i);}

        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr const T& at(const Int i) const {return this->_list.right_at(i);}


        /// @brief Masked indexing
        /// @param mask the mask that selects which elements to get. `true` will be included, `false` excluded
        /// @return A ArrayList that contains all values where of this where mask is `true`
        ArrayList<T, N> at(const ArrayList<bool, N>& mask) const {return this->_list.right_at(mask);}

        /// @brief Inices list indexing
        /// @tparam Int a generic integer
        /// @param indices a list of indices that should be extracted
        /// @return a ArrayList containing all the elements from this that are contained in the `indices`
        template<std::integral Int>
        ArrayList<T, N> at(const ArrayList<Int, N>& indices) const {return this->_list.right_at(indices);}

        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr T& operator[](const Int i){return this->right_at(i);}
        
        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr const T& operator[](const Int i) const {return this->right_at(i);}

        /// @brief accesses all elements where `mask` is `true` 
        ArrayList<T, N> operator[](const ArrayList<bool, N>& mask) const {return this->right_at(mask);}

        /// @brief accesses all elements at the given `indices` 
        template<std::integral Int>
        ArrayList<T, N> operator[](const ArrayList<Int, N>& indices) const {return this->right_at(indices);}

        /// @brief emplaces (aka. pushes) an element to the back of the list
        /// @details Actually constructs an element in place
        /// @tparam ...Args list of parameters that correspond to a constructor of the value type of the list
        /// @param ...args list of arguments to construct a value of the list
        /// @return a reference to the constructed list element
        template<class... Args>
        T& emplace_back(Args&&... args){return this->_list.right_emplace_back(std::forward<Args>(args)...);}

        /// @brief clears the list - destructs all members if necessary and sets the size to zero
        void clear(){this->_list.right_clear();}

        /// @brief appends `value` `count` many times
        /// @param count how often `value` should be emplaced_back()
        /// @param value the value to be created
        inline void append(const size_type count, const T& value){this->_list.right_append(count, value);}

        /// @brief assigns a value to the list. after which value is the only element in the list
        inline void assign(const size_type count, const T& value){this->_list.right_assign(count, value);}

        /// @brief appends a range defined by foreward iterators using the closed-open principle [first, last)
        template<std::forward_iterator Itr>
        inline void append(Itr first, Itr last){this->_list.right_append(first, last);}

        /// @brief assigns a range defined by foreward iterators using the closed-open principle [first, last). After the assignment the list has the size of the assigned range
        template<std::forward_iterator Itr>
        inline void assign(Itr first, Itr last){this->_list.right_assign(first, last);}

        /// @brief appens an initilizer_list
        inline void append(std::initializer_list<T> ilist){this->_list.right_append(ilist);}

        /// @brief assigns an initializer_list
        inline void assign(std::initializer_list<T> ilist){this->_list.right_assign(ilist);}

        /// @brief Appends the content of a range
        template<std::ranges::forward_range Range>
        inline void append(const Range& range){this->_list.right_append(range);}

        /// @brief Assigns the content of a range
        template<std::ranges::forward_range Range>
        inline void assign(const Range& range){this->_list.right_assign(range);}

        /// @brief turns the passed position given by an iterator into an integer 
        constexpr size_type to_index(const const_iterator pos) const {return this->_list.right_to_index(pos);}

        /// @brief turns the passed unsigned integer into an iterator pointing to the same position 
        template<std::integral Int>
        constexpr iterator to_iterator(const Int pos){return this->_list.right_to_iterator(pos);}

        /// @brief turns the passed unsigned integer into a cosnt_iterator pointing to the same position 
        template<std::integral Int>
        constexpr const_iterator to_iterator(const Int pos) const {return this->_list.right_to_iterator(pos);}

        /// @brief turns the passed unsigned integer into a cosnt_iterator pointing to the same position 
        template<std::integral Int>
        constexpr const_iterator to_const_iterator(const Int pos) const {return this->_list.right_to_const_iterator(pos);}

        /// @brief removes the constnes of an iterator if the user has access to the mutable (un-const) container 
        constexpr iterator unconst(const const_iterator pos) {return this->_list.right_unconst(pos);}

        /// @brief Inserts a value
        /// @param pos the position at which the value should be inserted
        /// @param value the value that the element at that position should have after the insertion
        /// @return an iterator pointing to the inserted value
        iterator insert(const const_iterator pos, const T& value){return this->_list.right_insert(pos, value);}

        /// @brief Inserts a value
        /// @param pos the position at which the value should be inserted
        /// @param value the value that the element at that position should have after the insertion
        /// @return an iterator pointing to the inserted value
        iterator insert(const const_iterator pos, T&& value){return this->_list.right_insert(pos, std::move(value));}

        /// @brief inserts a range at a given position
        /// @tparam Itr a generic iterator that follows the concept std::forward_iterator
        /// @param pos the position at which should be inserted given by an iterator
        /// @param first the start of the range that should be inserted
        /// @param last the past the end iterator to which should be inserted
        /// @return an iterator to the start of the start of the insertion
        template<std::forward_iterator Itr>
        iterator insert(const const_iterator pos, Itr first, Itr last){return this->_list.right_insert(pos, first, last);}

        /// @brief inserts a range at the given position
        /// @tparam Range a generic range that follows the concept `std::ranges::forward_range`
        /// @param pos the insertion point
        /// @param range the range that should be inserted
        /// @return an iterator to the inserted range
        template<std::ranges::forward_range Range>
        inline iterator insert(const const_iterator pos, const Range& range){return this->_list.right_insert(pos, range);}


        inline iterator insert(const const_iterator pos, std::initializer_list<T> ilist){return this->_list.right_insert(pos, ilist);}

        /// @brief inserts the `value` at the `pos`ition passed as an integer that wraps if it is a signed type 
        template<std::integral Int>
        inline iterator insert(const Int pos, const T& value){return this->_list.right_insert(pos, value);}

        /// @brief inserts the `value` at the `pos`ition passed as an integer that wraps if it is a signed type
        template<std::integral Int>
        inline iterator insert(const Int pos, T&& value){return this->_list.right_insert(pos, std::move(value));}

        /// @brief inserts the `range` at the `pos`ition passed as an integer that wraps if it is a signed type
        template<std::integral Int, std::ranges::forward_range Range>
        inline iterator insert(const Int pos, const Range& range){return this->_list.right_insert(pos, range);}
        
        template<std::integral Int>
        inline iterator insert(const Int pos, std::initializer_list<T> ilist){return this->_list.right_insert(pos, ilist);}

        /// @brief inserts the closed-open [`first`, `last`) range at the given `pos`ition
        template<std::integral Int, std::forward_iterator Itr>
        inline iterator insert(const Int pos, Itr first, Itr last){return this->_list.right_insert(pos, first, last);}

        /// @brief erases/removes the element at the position pointed to by `cpos` 
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed one
        iterator erase(const_iterator cpos){return this->_list.right_erase(cpos);}

        /// @brief erases/removes the element at the position pointed to by `cpos` 
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed one
        template<std::integral Int>
        inline iterator erase(const Int pos){return this->_list.right_erase(pos);}

        /// @brief erases/removes the range given by the closed-open iterators [first, last)
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed ones
        iterator erase(const_iterator first, const_iterator last){return this->_list.right_erase(first, last);}

        /// @brief erases elements from the list if they satisfy the callable
        /// @tparam Callable Object that can be called like `bool f(const T&)` or `bool f(T)`.
        /// @param f Condition that returns `true` if that element should be erased from the list.
        template<class Callable>
        std::size_t erase_if(Callable&& f){return this->_list.right_erase_if(f);}

        /// @brief erases/removes the range given by the closed-open indices [first, last)
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed ones
        template<std::integral Int>
        iterator erase(Int first, Int last){return this->_list.right_erase(first, last);}

        /// @brief removes and destructs the last element
        void pop_back(){this->_list.right_pop_back();}

        /// @brief Applies the negation (!) operator to all elements and returns it as a bool list 
        ArrayList<bool, N> operator!() const {
            ArrayList<bool, N> result;
            for(const auto& elem : *this){
                result.emplace_back(!elem);
            }
            return result;
        }

        /// @brief Applies the function to each element of the list in-place
        /// @note This is an in-place operation and will change the list. If you want to create a new list with the transformed values, use `embed::for_each(const ArrayList<T, N>&, std::function<T, const T&> function)` instead
        /// @param function the function being applied to change/transform each element
        template<class Function>
        constexpr void for_each(Function&& function){this->_list.right_for_each(function);}

    };




} // namespace embed

