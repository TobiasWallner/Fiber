#pragma once

#include <cstddef>
#include <utility>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <ranges>
#include <functional>
#include <concepts>

#include <embed/Exceptions/Exceptions.hpp>
#include <embed/OStream/OStream.hpp>


// TODO: use the memcpy - to potentially use dma copy
// TODO: Make a ArrayListSlice that stores pointers and feels like woring with references to the elements of another list.
// TODO: Make a reference type (ArrayListRef) that references the whole list with a pointer to the data, a pointer to its size, an int of the capacity
// TODO: also make a const reference version of both slice and ref
namespace embed
{
    /** 
     * @brief An array of contiguous memory which is statically allocated
     * 
     * 
     * Failure Modes and Effects Analysis
     * ----------------------------------
     * 
     * @tparam T type of the data elements
     * @tparam N size/count of the elements in the container
     * */ 
    template<class T, std::size_t N>
    class ArrayList{
    public:
        using value_type = T;
        using size_type = std::size_t;
        using reference = T&;
        using const_reference = const T&;
        using iterator = T*;
        using const_iterator = const T*;
        using pointer = T*;
        using const_pointer = T*;

    private:
        alignas(T) std::byte _buffer[N * sizeof(T)];
        size_type _size = 0;
        static constexpr size_type _capacity = N;

    public:

        /// @brief default constructor
        ArrayList() = default;

        /// @brief default copy constructor
        template<std::convertible_to<T> Ta, size_t N1>
        ArrayList(const ArrayList<Ta, N1>& other){this->assign(other);}

        /// @brief default copy assignment 
        template<std::convertible_to<T> Ta, size_t N1>
        ArrayList& operator=(const ArrayList<Ta, N1>& other){this->assign(other); return *this;}

        /// @brief construct from an initialiser list
        /// @param ilist initialiser list
        template<std::convertible_to<T> Ta>
        inline ArrayList(std::initializer_list<Ta> ilist){this->append(ilist);}

        /// @brief construct from an generic range that follows the concept `std::ranges::forward_range`
        /// @tparam Range templated range class that follows the concept `std::ranges::forward_range`
        /// @param range the range that should be assigned on construction
        template<std::ranges::forward_range Range>
        inline ArrayList(const Range& range){this->append(range);}

        /// @brief destructor
        ~ArrayList(){
            if constexpr (!std::is_trivially_destructible<T>::value){
                for(T& elem : *this){
                    elem->~T();
                }
            }
        }

        /// @brief returns the size/count of live elements in the container 
        constexpr size_type size() const {return this->_size;}

        /// @brief returns the capacity of the container. Since this is a statically allocated container this is also the maximal size.
        constexpr size_type capacity() const {return this->_capacity;}

        /// @brief returns the maximal number of elements that can be stored in the container 
        constexpr size_type max_size() const {return this->_capacity;}

        /// @brief returns the reserve - number of elements that can be stored until the container is full 
        constexpr size_type reserve() const {return this->capacity() - this->size();}

        /// @brief returns true if there are not elements in the container, aka. the container is empty. 
        constexpr bool empty() const {return this->size() == 0;}

        /// @brief returns true if the container is full and no more elements can be stored in the container 
        constexpr bool full() const {return this->size() == this->capacity();}

        /// @brief returns a pointer to the start of the container
        constexpr pointer data() {return reinterpret_cast<T*>(this->_buffer);}

        /// @brief returns a const-pointer to the start of the container 
        constexpr const_pointer data() const {return reinterpret_cast<const T*>(this->_buffer);}

        /// @brief returns a const-pointer to the start of the container 
        constexpr const_pointer cdata() const {return reinterpret_cast<const T*>(this->_buffer);}

        /// @brief returns an iterator to the start 
        constexpr iterator begin() {return reinterpret_cast<T*>(this->_buffer);}

        /// @brief returns a const-iterator to the start 
        constexpr const_iterator begin() const {return reinterpret_cast<const T*>(this->_buffer);}

        /// @brief returns a const-iterator to the start 
        constexpr const_iterator cbegin() const {return reinterpret_cast<const T*>(this->_buffer);}

        /// @brief returns an iterator past the end 
        constexpr iterator end() {return this->begin() + this->size();}

        /// @brief returns a const-iterator past the end
        constexpr const_iterator end() const {return this->begin() + this->size();}

        /// @brief returns a const-iterator past the end
        constexpr const_iterator cend() const {return this->cbegin() + this->size();}

        /// @brief returns a reference to the first element in the buffer
        constexpr reference front() {
            EMBED_ASSERT_O1(!this->empty());
            return *this->begin();
        }

        /// @brief returns a const-reference to the first element int the buffer
        constexpr const_reference front() const {
            EMBED_ASSERT_O1(!this->empty());
            return *this->cbegin();
        }

        /// @brief returns a reference to the last element in the buffer 
        constexpr reference back() {
            EMBED_ASSERT_O1(!this->empty());
            return *(this->end()-1);
        }

        /// @brief returns a const reference to the last element in the buffer 
        constexpr const_reference back() const {
            EMBED_ASSERT_O1(!this->empty());
            return *(this->cend()-1);
        }

        /**
         * @brief returns a reference to the element at the given position
         * 
         * For signed integer types, signed ones will wrap negatives around so -1 will access the last element
         * Unsigned integer types will avoid the branch.
         */
        template<std::integral Int>
        constexpr reference at(const Int i) {
            if constexpr (std::is_unsigned_v<Int>){
                EMBED_ASSERT_O1(i < this->size());  
                return *(this->begin()+i);
            }else{
                EMBED_ASSERT_O1(i < this->size());
                embed::cout << "-i: " << (-i) << embed::endl;
                embed::cout << "this->size(): " << this->size() << embed::endl;
                embed::cout << "-i <= this->size(): " << (-i <= this->size()) << embed::endl;
                embed::cout << "-------------------------" << embed::endl; 

                EMBED_ASSERT_O1(-i <= this->size());
                return *(((i >= 0) ? this->begin() : this->end()) + i);
            }   
        }

        /// @overload 
        template<std::integral Int>
        constexpr const_reference at(const Int i) const {
            if constexpr (std::is_unsigned_v<Int>){
                EMBED_ASSERT_O1(i < this->size());  
                return *(this->begin()+i);
            }else{
                EMBED_ASSERT_O1(i < this->size());
                EMBED_ASSERT_O1(-i <= this->size());
                return *(((i >= 0) ? this->begin() : this->end()) + i);
            }   
        }


        /// @brief Masked indexing
        /// @param mask the mask that selects which elements to get. `true` will be included, `false` excluded
        /// @return A ArrayList that contains all values where of this where mask is `true`
        ArrayList<T, N> at(const ArrayList<bool, N>& mask) const {
            ArrayList<T, N> result;
            auto thisItr = this->begin();
            const auto thisEnd = this->end();
            auto maskItr = mask.begin();
            const auto maskEnd = mask.end();
            for(; thisItr < thisEnd && maskItr < maskEnd; ++thisItr, (void)++maskItr){
                if(*maskItr){
                    result.emplace_back(*thisItr);
                }
            }
            return result;
        }

        /// @brief Indices list indexing
        /// @tparam Int a generic integer
        /// @param indices a list of indices that should be extracted
        /// @return a ArrayList containing all the elements from this that are contained in the `indices`
        template<std::integral Int>
        ArrayList<T, N> at(const ArrayList<Int, N>& indices) const {
            ArrayList<T, N> result;
            for(const Int& index : indices){
                result.emplace_back(this->at(index));
            }
            return result;
        }

        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr reference operator[](const Int i){return this->at(i);}
        
        /// @brief returns a reference to the element at the given position
        template<std::integral Int>
        constexpr const_reference operator[](const Int i) const {return this->at(i);}

        /// @brief accesses all elements where `mask` is `true` 
        ArrayList<T, N> operator[](const ArrayList<bool, N>& mask) const {return this->at(mask);}

        /// @brief accesses all elements at the given `indices` 
        template<std::integral Int>
        ArrayList<T, N> operator[](const ArrayList<Int, N>& indices) const {return this->at(indices);}

        /// @brief emplaces (aka. pushes) an element to the back of the list
        /// @details Actually constructs an element in place
        /// @tparam ...Args list of parameters that correspond to a constructor of the value type of the list
        /// @param ...args list of arguments to construct a value of the list
        /// @return a reference to the constructed list element
        template<class... Args>
        T& emplace_back(Args&&... args){
            EMBED_ASSERT_O1(!this->full());
            T* construct_at_addr = this->begin() + this->size();
            new (construct_at_addr) T(std::forward<Args>(args)...);
            this->_size += 1;
            return this->back();
        }

        /// @brief clears the list - destructs all members if necessary and sets the size to zero
        void clear(){
            if constexpr (!std::is_trivially_destructible<T>::value){
                for(T& elem : *this){
                    elem->~T();
                }
            }
            this->_size = 0;
        }

        /// @brief appends `value` `count` many times
        /// @param count how often `value` should be emplaced_back()
        /// @param value the value to be created
        inline void append(const size_type count, const T& value){
            for(size_type i = 0; i < count; ++i) this->emplace_back(value);
        }

        /// @brief clears the list and assigns the `value` `count` many times to the list.
        inline void assign(const size_type count, const T& value){
            this->clear();
            this->append(count, value);
        }

        /// @brief appends a range defined by foreward iterators using the closed-open principle [first, last)
        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        inline void append(Itr first, Itr last){
            for(; first != last; ++first) 
                this->emplace_back(*first);
        }

        /// @brief assigns a range defined by foreward iterators using the closed-open principle [first, last). After the assignment the list has the size of the assigned range
        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        inline void assign(Itr first, Itr last){
            this->clear();
            this->append(first, last);
        }

        /// @brief appens an initilizer_list
        template<std::convertible_to<T> Ta>
        inline void append(std::initializer_list<Ta> ilist){
            this->append(ilist.begin(), ilist.end());
        }

        /// @brief assigns an initializer_list
        template<std::convertible_to<T> Ta>
        inline void assign(std::initializer_list<Ta> ilist){
            this->clear();
            this->append(ilist);
        }

        /// @brief Appends the content of a range
        template<std::ranges::forward_range Range>
        inline void append(const Range& range){
            this->append(range.begin(), range.end());
        }

        /// @brief Assigns the content of a range
        template<std::ranges::forward_range Range>
        inline void assign(const Range& range){
            this->clear();
            this->append(range);
        }

        /// @brief turns the passed position given by an iterator into an integer 
        constexpr size_type to_index(const const_iterator pos) const {
            EMBED_ASSERT_O1(this->begin() <= pos && pos < this->end());
            return pos - this->begin();
        }

        /// @brief turns the passed unsigned integer into an iterator pointing to the same position 
        template<std::integral Int>
        constexpr iterator to_iterator(const Int pos){
            if constexpr (std::is_unsigned_v<Int>){
                EMBED_ASSERT_O1(pos < this->size());
                return this->begin() + pos;
            }else{
                EMBED_ASSERT_O1(pos < this->size());
                EMBED_ASSERT_O1(-pos <= this->size());
                return ((pos >= 0) ? this->begin() : this->end()) + pos;
            }
        }

        /// @brief turns the passed unsigned integer into a cosnt_iterator pointing to the same position 
        template<std::integral Int>
        constexpr const_iterator to_iterator(const Int pos) const {
            if constexpr (std::is_unsigned_v<Int>){
                EMBED_ASSERT_O1(pos < this->size());
                return this->begin() + pos;
            }else{
                EMBED_ASSERT_O1(pos < this->size());
                EMBED_ASSERT_O1(-pos <= this->size());
                return ((pos >= 0) ? this->begin() : this->end()) + pos;
            }
        }

        /// @brief turns the passed unsigned integer into a cosnt_iterator pointing to the same position 
        template<std::integral Int>
        constexpr const_iterator to_const_iterator(const Int pos) const {
            if constexpr (std::is_unsigned_v<Int>){
                EMBED_ASSERT_O1(pos < this->size());
                return this->begin() + pos;
            }else{
                EMBED_ASSERT_O1(pos < this->size());
                EMBED_ASSERT_O1(-pos <= this->size());
                return ((pos >= 0) ? this->begin() : this->end()) + pos;
            }
        }

        /// @brief removes the constnes of an iterator if the user has access to the mutable (un-const) container 
        constexpr iterator unconst(const const_iterator pos) {
            EMBED_ASSERT_O1(this->begin() <= pos);
            EMBED_ASSERT_O1(pos < this->end());
            return this->begin() + this->to_index(pos);
        }

        /// @brief Inserts a value
        /// @tparam Ta A type that is convertible to the `value_type` of the list
        /// @param pos the position at which the value should be inserted
        /// @param value the value that the element at that position should have after the insertion
        /// @return an iterator pointing to the inserted value
        template<std::convertible_to<T> Ta>
        iterator insert(const const_iterator pos, const Ta& value){
            EMBED_ASSERT_O1(!this->full());
            for(auto i = this->end(); i != pos; --i) *i = std::move(*(i-1));
            this->_size += 1;
            iterator pos_ = unconst(pos);
            *pos_ = value;
            return pos_;
        }

        /// @brief Inserts a value
        /// @param pos the position at which the value should be inserted
        /// @param value the value that the element at that position should have after the insertion
        /// @return an iterator pointing to the inserted value
        template<std::convertible_to<T> Ta>
        iterator insert(const const_iterator pos, Ta&& value){
            EMBED_ASSERT_O1(!this->full());
            for(auto i = this->end(); i != pos; --i) *i = std::move(*(i-1));
            this->_size += 1;
            iterator pos_ = unconst(pos);
            *pos_ = std::move(value);
            return pos_;
        }

        /// @brief inserts a range at a given position
        /// @tparam Itr a generic iterator that follows the concept std::forward_iterator
        /// @param pos the position at which should be inserted given by an iterator
        /// @param first the start of the range that should be inserted
        /// @param last the past the end iterator to which should be inserted
        /// @return an iterator to the start of the start of the insertion
        template<std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        iterator insert(const const_iterator pos, Itr first, Itr last){
            size_type dist = std::distance(first, last);
            EMBED_ASSERT_O1(dist > this->reserve());
            for(auto i = this->end(); i != pos; --i) *(i + dist - 1) = std::move(*(i - 1));
            iterator insertIterator = this->unconst(pos);
            for(; first != last; ++first, (void)++insertIterator) *insertIterator = *first;
            this->_size += dist;
            return unconst(pos);
        }

        /// @brief inserts a range at the given position
        /// @tparam Range a generic range that follows the concept `std::ranges::forward_range`
        /// @param pos the insertion point
        /// @param range the range that should be inserted
        /// @return an iterator to the inserted range
        template<std::ranges::forward_range Range>
        inline iterator insert(const const_iterator pos, const Range& range){
            return this->insert(pos, range.begin(), range.end());
        }

        /// @brief inserts the `value` at the `pos`ition passed as an integer that wraps if it is a signed type 
        template<std::integral Int>
        inline iterator insert(const Int index, const T& value){
            if constexpr (std::is_unsigned_v<Int>){
                return this->insert(this->begin() + index, value);
            }else{
                if(index >= 0){
                    return this->insert(this->begin() + index, value);
                }else{
                    return this->insert(this->end() + index + 1, value);
                }
            }
        }

        /// @brief inserts the `value` at the `pos`ition passed as an integer that wraps if it is a signed type
        template<std::integral Int, std::convertible_to<T> Ta>
        inline iterator insert(const Int index, Ta&& value){
            if constexpr (std::is_unsigned_v<Int>){
                return this->insert(this->begin() + index, std::move(value));
            }else{
                if(index >= 0){
                    return this->insert(this->begin() + index, std::move(value));
                }else{
                    return this->insert(this->end() + index + 1, std::move(value));
                }
            }
        }

        /// @brief inserts the `range` at the `pos`ition passed as an integer that wraps if it is a signed type
        template<std::integral Int, std::ranges::forward_range Range>
        inline iterator insert(const Int index, const Range& range){
            if constexpr (std::is_unsigned_v<Int>){
                return this->insert(this->begin() + index, range);
            }else{
                if(index >= 0){
                    return this->insert(this->begin() + index, range);
                }else{
                    return this->insert(this->end() + index + 1, range);
                }
            }
        }

        /// @brief inserts the closed-open [`first`, `last`) range at the given `pos`ition
        template<std::integral Int, std::forward_iterator Itr>
        requires std::convertible_to<typename std::iterator_traits<Itr>::value_type, T>
        inline iterator insert(const Int index, Itr first, Itr last){
            if constexpr (std::is_unsigned_v<Int>){
                return this->insert(this->begin() + index, first, last);
            }else{
                if(index >= 0){
                    return this->insert(this->begin() + index, first, last);
                }else{
                    return this->insert(this->end() + index + 1, first, last);
                }
            }
        }

        /// @brief erases/removes the element at the position pointed to by `cpos` 
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed one
        iterator erase(const_iterator cpos){
            EMBED_ASSERT_O1(!this->empty());
            iterator destItr = this->unconst(cpos);
            iterator sourceItr = destItr + 1;
            for(; sourceItr < this->end(); ++sourceItr, (void)++destItr) *destItr = std::move(*sourceItr);
            if constexpr (!std::is_trivially_destructible_v<T>){
                this->back().~T();
            }
            this->_size -= 1;
            return this->unconst(cpos);
        }

        /// @brief erases/removes the element at the position pointed to by `cpos` 
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed one
        template<class Int> requires std::is_integral_v<Int>
        inline iterator erase(const Int pos){
            return this->erase(this->to_iterator(pos));
        }

        /// @brief erases/removes the range given by the closed-open iterators [first, last)
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed ones
        iterator erase(const_iterator first, const_iterator last){
            EMBED_ASSERT_O1(this->begin() <= first);
            EMBED_ASSERT_O1(first < this->end());
            EMBED_ASSERT_O1(this->begin() <= last);
            EMBED_ASSERT_O1(last < this->end());
            iterator destItr = this->unconst(first);
            iterator sourceItr = this->unconst(last);
            for(; (destItr != last) && (sourceItr != this->end()); ++destItr, (void)++sourceItr) *destItr = std::move(*sourceItr);
            
            if constexpr (!std::is_trivially_destructible_v<T>){
                for(; destItr != this->end(); ++destItr) destItr->~T();
            }
            
            const auto dist = std::distance(first, last);
            this->_size -= dist;
            return this->unconst(last-dist);
        }

        /// @brief erases elements from the list if they satisfy the callable
        /// @tparam Callable Object that can be called like `bool f(const T&)` or `bool f(T)`.
        /// @param f Condition that returns `true` if that element should be erased from the list.
        template<class Callable>
        std::size_t erase_if(Callable&& f){
            iterator read_itr = this->begin();
            const iterator end_itr = this->end();

            // search first that is true
            for(;read_itr != end_itr; ++read_itr){
                if(f(*read_itr)) break;
            }

            // return if it did not aplie to any
            if(read_itr == end_itr) return 0;
            
            // setup first read and write iterators
            iterator write_itr = read_itr;
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
            std::size_t new_size = std::distance(this->begin(), write_itr);
            std::size_t n_erased = std::distance(write_itr, this->end());

            // destroy remaining
            if constexpr (!std::is_trivially_destructible_v<T>){
                for(; write_itr != end_itr; ++write_itr){
                    write_itr->~T();
                }
            }
            
            this->_size = new_size;
            return n_erased;
        }

        /// @brief erases/removes the range given by the closed-open indices [first, last)
        /// @details does not perform out of bounds checks
        /// @returns an iterator the element after the removed ones
        template<class Int> requires std::is_integral_v<Int>
        iterator erase(Int first, Int last){return this->erase(this->to_iterator(first), this->to_iterator(last));}

        /// @brief removes and destructs the last element
        void pop_back(){
            EMBED_ASSERT_O1(!this->empty());
            if constexpr (!std::is_trivially_destructible<T>::value){
                this->back().~T();
            }
            this->_size -= 1;
        }

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
        constexpr void for_each(Function&& function){
            for(auto& elem : *this){
                elem = function(elem);
            }
        }
    };

    template<class T, std::size_t N, class Function>
    auto for_each(const ArrayList<T, N>& list, Function&& function){
        using R = decltype(function(std::declval<T>()));
        ArrayList<R, N> result;
        for(const T& elem : list){
            result.emplace_back(function(elem));
        }
        return result;
    }

    // --------------------------------------------------------------------------------------
    //                              Formated stream outpuot
    // --------------------------------------------------------------------------------------

    /// @brief prints the array list to the output stream  
    template<class T, std::size_t N>
    OStream& operator<<(OStream& stream, const ArrayList<T, N>& array){
        stream << "[";
        bool is_first = true;
        for(const auto& elem : array){
            if(is_first){
                is_first = false;
            }else{
                stream << ", ";
            }
            stream << elem;
        }
        return stream << "]";
    }

    // --------------------------------------------------------------------------------------
    //                    basic comparisons of scalar types
    // --------------------------------------------------------------------------------------

    template<class T1, class T2> constexpr bool skalar_equal(const T1& l, const T2& r){return l == r;}
    template<class T1, class T2> constexpr bool skalar_not_equal(const T1& l, const T2& r){return l != r;}
    template<class T1, class T2> constexpr bool skalar_less(const T1& l, const T2& r){return l < r;}
    template<class T1, class T2> constexpr bool skalar_greater(const T1& l, const T2& r){return l > r;}
    template<class T1, class T2> constexpr bool skalar_less_equal(const T1& l, const T2& r){return l <= r;}
    template<class T1, class T2> constexpr bool skalar_greater_equal(const T1& l, const T2& r){return l >= r;}

    // --------------------------------------------------------------------------------------
    //                    reduction comparisons for cintiguous memory
    // --------------------------------------------------------------------------------------

    /// @brief compares the ranges given by closed-open iterators
    /// @tparam T1 value type of the left ranges
    /// @tparam T2 value type of the right ranges
    /// @param afirst start iterator of the left range
    /// @param alast end iterator of the left range
    /// @param bfirst start iterator of the right range
    /// @param blast end iterator of the right range
    /// @param compare a function that compares them - the function returns true if `compare` is true for all elements
    /// @param Default the default value that should be returned in case the ranges do not have the same number of elements
    /// @return returns the default if the ranges do not have the same size. if they have the same size returns true if the compare function evalues true for all elements
    template<class T1, class T2>
    constexpr bool compare(const T1* afirst, const T1* alast, const T2* bfirst, const T2* blast, std::function<bool(const T1& l, const T2& r)> compare, bool Default=false){
        if(std::distance(afirst, alast) == std::distance(bfirst, blast)){
            for(; afirst != alast; ++afirst, (void)++bfirst){
                if(!compare(*afirst, *bfirst)) return false;
            }
            return true;
        }
        return Default;
    }

    /// @brief returns true if both ranges are equal in value and count 
    template<class T1, class T2>
    constexpr bool equal(const T1* afirst, const T1* alast, const T2* bfirst, const T2* blast){
        return compare<T1, T2>(afirst, alast, bfirst, blast, skalar_equal<T1, T2>);
    }

    /// @brief returns true if the ranges are not equal in value nor count
    template<class T1, class T2>
    constexpr bool not_equal(const T1* afirst, const T1* alast, const T2* bfirst, const T2* blast){
        return compare<T1, T2>(afirst, alast, bfirst, blast, skalar_not_equal<T1, T2>, true);
    }

    /// @brief returns true if the ranges are equal in count and the left ranges value are all smaller than the right ones 
    template<class T1, class T2>
    constexpr bool less(const T1* afirst, const T1* alast, const T2* bfirst, const T2* blast){
        return compare<T1, T2>(afirst, alast, bfirst, blast, skalar_less<T1, T2>);
    }

    
    /// @brief returns true if the ranges are equal in count and the left ranges value are all greater than the right ones
    template<class T1, class T2>
    constexpr bool greater(const T1* afirst, const T1* alast, const T2* bfirst, const T2* blast){
        return compare<T1, T2>(afirst, alast, bfirst, blast, skalar_greater<T1, T2>);
    }

    /// @brief returns true if the ranges are equal in count and the left ranges value are all less or equal to the right ones 
    template<class T1, class T2>
    constexpr bool less_equal(const T1* afirst, const T1* alast, const T2* bfirst, const T2* blast){
        return compare<T1, T2>(afirst, alast, bfirst, blast, skalar_less_equal<T1, T2>);
    }

    
    /// @brief returns true if the ranges are equal in count and the left ranges value are all greater or equal to the right ones 
    template<class T1, class T2>
    constexpr bool greater_equal(const T1* afirst, const T1* alast, const T2* bfirst, const T2* blast){
        return compare<T1, T2>(afirst, alast, bfirst, blast, skalar_greater_equal<T1, T2>);
    }

    // --------------------------------------------------------------------------------------
    //                        comparisons for StaticArrays
    // --------------------------------------------------------------------------------------
    
    /// @brief returns `true` if both containers have the same number of and value of elements
    template<class T1, class T2, std::size_t N1, std::size_t N2>
    inline bool equal(const ArrayList<T1, N1>& lhs, const ArrayList<T2, N2>& rhs){
        return equal<T1, T2>(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    /// @brief returns `true` if both containers do not have the same number of and value of elements
    template<class T1, class T2, std::size_t N1, std::size_t N2>
    inline bool not_equal(const ArrayList<T1, N1>& lhs, const ArrayList<T2, N2>& rhs){
        return not_equal<T1, T2>(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    /// @brief returns `true` if both containers have the same number of elements and the left one has point-wise lesser ones for all entries
    template<class T1, class T2, std::size_t N1, std::size_t N2>
    inline bool less(const ArrayList<T1, N1>& lhs, const ArrayList<T2, N2>& rhs){
        return less<T1, T2>(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    /// @brief returns `true` if both containers have the same number of elements and the left one has point-wise greater ones for all entries
    template<class T1, class T2, std::size_t N1, std::size_t N2>
    inline bool greater(const ArrayList<T1, N1>& lhs, const ArrayList<T2, N2>& rhs){
        return greater<T1, T2>(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    /// @brief returns `true` if both containers have the same number of elements and the left one has point-wise lesser or equal ones for all entries
    template<class T1, class T2, std::size_t N1, std::size_t N2>
    inline bool less_equal(const ArrayList<T1, N1>& lhs, const ArrayList<T2, N2>& rhs){
        return less_equal<T1, T2>(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    /// @brief returns `true` if both containers have the same number of elements and the left one has point-wise greater or equal ones for all entries
    template<class T1, class T2, std::size_t N1, std::size_t N2>
    inline bool greater_equal(const ArrayList<T1, N1>& lhs, const ArrayList<T2, N2>& rhs){
        return greater_equal<T1, T2>(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }


    // --------------------------------------------------------------------------------------
    //                    pointwise comparisons for cintiguous memory
    // --------------------------------------------------------------------------------------

    /// @brief performs a point wise comparison of the ArrayList
    /// @returns a boolean array list of the intersection of both lists with the result of the compare function
    template<class T, std::size_t N1, std::size_t N2>
    ArrayList<bool, (N1 < N2) ? N1 : N2> point_wise_compare(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs, std::function<bool(const T& l, const T& r)> compare){
        ArrayList<bool, (N1 < N2) ? N1 : N2> result;
        std::size_t limit = (lhs.size() < rhs.size()) ? lhs.size() : rhs.size();
        for(std::size_t i = 0; i < limit; ++i){
            result.emplace_back(compare(lhs[i], rhs[i]));
        }
        return result;
    }
    
    /// @brief returns the point wise equal list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto point_wise_equal(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_compare<T, N1, N2>(lhs, rhs, skalar_equal<T, T>);
    }

    /// @brief returns the point wise un-equal list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto point_wise_not_equal(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_compare<T, N1, N2>(lhs, rhs, skalar_not_equal<T, T>);
    }

    /// @brief returns the point wise less list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto point_wise_less(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_compare<T, N1, N2>(lhs, rhs, skalar_less<T, T>);
    }

    /// @brief returns the point wise greater list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto point_wise_greater(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_compare<T, N1, N2>(lhs, rhs, skalar_greater<T, T>);
    }

    /// @brief returns the point wise less equal list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto point_wise_less_equal(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_compare<T, N1, N2>(lhs, rhs, skalar_less_equal<T, T>);
    }

    /// @brief returns the point wise greater equal list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto point_wise_greater_equal(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_compare<T, N1, N2>(lhs, rhs, skalar_greater_equal<T, T>);
    }

    // ---------- comparison operators -----------

    /// @brief returns the point wise equal list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto operator==(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_equal<T, N1, N2>(lhs, rhs);
    }

    /// @brief returns the point wise un-equal list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto operator!=(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_not_equal<T, N1, N2>(lhs, rhs);
    }

    /// @brief returns the point wise less list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto operator<(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_less<T, N1, N2>(lhs, rhs);
    }

    /// @brief returns the point wise greater list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto operator>(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_greater<T, N1, N2>(lhs, rhs);
    }

    /// @brief returns the point wise less equal list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto operator<=(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_less_equal<T, N1, N2>(lhs, rhs);
    }

    /// @brief returns the point wise greater equal list of the left and right hand side
    template<class T, std::size_t N1, std::size_t N2>
    inline auto operator>=(const ArrayList<T, N1>& lhs, const ArrayList<T, N2>& rhs){
        return point_wise_greater_equal<T, N1, N2>(lhs, rhs);
    }

    // --------------- reduction operations ---------------

    /// @brief returns `true` if any (aka. at least one) values from the `list` (converted to bool) are `true`
    template<class T, std::size_t N1>
    constexpr bool any(const ArrayList<T, N1>& list){
        for(const auto& elem : list){
            if(static_cast<bool>(elem)){
                return true;
            }
        }
        return false;
    }

    /// @brief returns `true` if all values from the `list` (converted to bool) are `true`
    template<class T, std::size_t N1>
    constexpr bool all(const ArrayList<T, N1>& lhs){
        for(const auto& elem : lhs){
            if(!static_cast<bool>(elem)){
                return false;
            }
        }
        return true;
    }

    /// @brief returns `true` if no values from the `list` (converted to bool) are `true`
    template<class T, std::size_t N1>
    constexpr bool none(const ArrayList<T, N1>& lhs){
        for(const auto& elem : lhs){
            if(static_cast<bool>(elem)){
                return false;
            }
        }
        return true;
    }

} // namespace embed


