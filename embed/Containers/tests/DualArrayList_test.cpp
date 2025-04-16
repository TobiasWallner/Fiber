#include "DualArrayList_test.hpp"

#include <embed/Containers/DualArrayList.hpp>
#include <embed/OStream/OStream.hpp>
#include <embed/test/test.hpp>

namespace embed{

    namespace{
        void construction(){

            DualArrayList<int, 5> a;

            LeftDualArrayListRef la = a;
            RightDualArrayListRef ra = a;

            TEST_TRUE(a.empty());
            TEST_FALSE(a.full());
            TEST_EQUAL(a.left_size(), 0);
            TEST_EQUAL(a.right_size(), 0);
            TEST_EQUAL(la.size(), 0);
            TEST_EQUAL(ra.size(), 0);
            TEST_EQUAL(a.max_size(), 5);
            TEST_EQUAL(a.left_capacity(), 5);
            TEST_EQUAL(a.left_capacity(), la.capacity());
            TEST_EQUAL(a.right_capacity(), 5);
            TEST_EQUAL(a.right_capacity(), ra.capacity());
            TEST_EQUAL(a.left_begin(), a.left_end());
            TEST_EQUAL(a.left_begin(), la.begin());
            TEST_EQUAL(a.left_end(), la.end());
            TEST_EQUAL(a.right_begin(), a.right_end());
            TEST_EQUAL(a.right_begin(), ra.begin());
            TEST_EQUAL(a.right_end(), ra.end());
            TEST_NOT_EQUAL(a.left_begin(), a.right_begin().base()-1);
            TEST_EQUAL(a.reserve(), 5);

            embed::cout << "  finished: " << __func__ << embed::endl;
        }

        void emplace_back(){
            DualArrayList<int, 4> a;
            LeftDualArrayListRef la = a;
            RightDualArrayListRef ra = a;

            // insertion into a-left
            a.left_emplace_back(5);

            TEST_FALSE(a.empty());
            TEST_FALSE(a.full());
            TEST_EQUAL(a.left_size(), 1);
            TEST_EQUAL(a.right_size(), 0);
            TEST_FALSE(a.left_empty());
            TEST_TRUE(a.right_empty());
            TEST_EQUAL(a.left_front(), 5);
            TEST_EQUAL(a.left_back(), 5);
            TEST_NOT_EQUAL(a.left_begin(), a.left_end());
            TEST_EQUAL(a.right_begin(), a.right_end());
            TEST_EQUAL(a.left_capacity(), 4);
            TEST_EQUAL(a.right_capacity(), 3);
            TEST_EQUAL(a.reserve(), 3);

            
            TEST_EQUAL(la.size(), 1);
            TEST_EQUAL(ra.size(), 0);
            TEST_FALSE(la.empty());
            TEST_TRUE(ra.empty());
            TEST_FALSE(la.full());
            TEST_FALSE(ra.full());
            TEST_EQUAL(la.front(), 5);
            TEST_EQUAL(la.back(), 5);
            TEST_NOT_EQUAL(la.begin(), la.end());
            TEST_EQUAL(ra.begin(), ra.end());
            TEST_EQUAL(la.capacity(), 4);
            TEST_EQUAL(ra.capacity(), 3);
            TEST_EQUAL(la.reserve(), 3);
            TEST_EQUAL(la.reserve(), 3);

            // insertion into a-right


            a.right_emplace_back(8);

            TEST_FALSE(a.empty());
            TEST_FALSE(a.full());
            TEST_EQUAL(a.left_size(), 1);
            TEST_EQUAL(a.right_size(), 1);
            TEST_FALSE(a.left_empty());
            TEST_FALSE(a.right_empty());
            TEST_EQUAL(a.left_front(), 5);
            TEST_EQUAL(a.left_back(), 5);
            TEST_EQUAL(a.right_front(), 8);
            TEST_EQUAL(a.right_back(), 8);
            TEST_NOT_EQUAL(a.left_begin(), a.left_end());
            TEST_NOT_EQUAL(a.right_begin(), a.right_end());
            TEST_EQUAL(a.left_capacity(), 3);
            TEST_EQUAL(a.right_capacity(), 3);
            TEST_EQUAL(a.reserve(), 2);

            TEST_EQUAL(la.size(), 1);
            TEST_EQUAL(ra.size(), 1);
            TEST_FALSE(la.empty());
            TEST_FALSE(ra.empty());
            TEST_FALSE(la.full());
            TEST_FALSE(ra.full());
            TEST_EQUAL(la.front(), 5);
            TEST_EQUAL(ra.front(), 8);
            TEST_NOT_EQUAL(la.begin(), la.end());
            TEST_NOT_EQUAL(ra.begin(), ra.end());
            TEST_EQUAL(la.capacity(), 3);
            TEST_EQUAL(ra.capacity(), 3);
            TEST_EQUAL(la.reserve(), 2);
            TEST_EQUAL(la.reserve(), 2);

            // insertion into reference a-left
            la.emplace_back(1);

            TEST_FALSE(a.empty());
            TEST_FALSE(a.full());
            TEST_EQUAL(a.left_size(), 2);
            TEST_EQUAL(a.right_size(), 1);
            TEST_FALSE(a.left_empty());
            TEST_FALSE(a.right_empty());
            TEST_EQUAL(a.left_front(), 5);
            TEST_EQUAL(a.left_back(), 1);
            TEST_EQUAL(a.right_front(), 8);
            TEST_EQUAL(a.right_back(), 8);
            TEST_NOT_EQUAL(a.left_begin(), a.left_end());
            TEST_NOT_EQUAL(a.right_begin(), a.right_end());
            TEST_EQUAL(a.left_capacity(), 3);
            TEST_EQUAL(a.right_capacity(), 2);
            TEST_EQUAL(a.reserve(), 1);

            TEST_EQUAL(la.size(), 2);
            TEST_EQUAL(ra.size(), 1);
            TEST_FALSE(la.empty());
            TEST_FALSE(ra.empty());
            TEST_FALSE(la.full());
            TEST_FALSE(ra.full());
            TEST_EQUAL(la.front(), 5);
            TEST_EQUAL(la.back(), 1);
            TEST_EQUAL(ra.front(), 8);
            TEST_EQUAL(ra.back(), 8);
            TEST_NOT_EQUAL(la.begin(), la.end());
            TEST_NOT_EQUAL(ra.begin(), ra.end());
            TEST_EQUAL(la.capacity(), 3);
            TEST_EQUAL(ra.capacity(), 2);
            TEST_EQUAL(la.reserve(), 1);
            TEST_EQUAL(la.reserve(), 1);

            // insertion into reference a right
            ra.emplace_back(2);

            TEST_FALSE(a.empty());
            TEST_TRUE(a.full());
            TEST_EQUAL(a.left_size(), 2);
            TEST_EQUAL(a.right_size(), 2);
            TEST_FALSE(a.left_empty());
            TEST_FALSE(a.right_empty());
            TEST_EQUAL(a.left_front(), 5);
            TEST_EQUAL(a.left_back(), 1);
            TEST_EQUAL(a.right_front(), 8);
            TEST_EQUAL(a.right_back(), 2);
            TEST_NOT_EQUAL(a.left_begin(), a.left_end());
            TEST_NOT_EQUAL(a.right_begin(), a.right_end());
            TEST_EQUAL(a.left_capacity(), 2);
            TEST_EQUAL(a.right_capacity(), 2);
            TEST_EQUAL(a.reserve(), 0);

            TEST_EQUAL(la.size(), 2);
            TEST_EQUAL(ra.size(), 2);
            TEST_FALSE(la.empty());
            TEST_FALSE(ra.empty());
            TEST_TRUE(la.full());
            TEST_TRUE(ra.full());
            TEST_EQUAL(la.front(), 5);
            TEST_EQUAL(la.back(), 1);
            TEST_EQUAL(ra.front(), 8);
            TEST_EQUAL(ra.back(), 2);
            TEST_NOT_EQUAL(la.begin(), la.end());
            TEST_NOT_EQUAL(ra.begin(), ra.end());
            TEST_EQUAL(la.capacity(), 2);
            TEST_EQUAL(ra.capacity(), 2);
            TEST_EQUAL(la.reserve(), 0);
            TEST_EQUAL(la.reserve(), 0);
        }

        void clear(){

            {// clear original left
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.left_emplace_back(3);
                a.right_emplace_back(4); 
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 
    
                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;
    
                a.left_clear();
    
                TEST_TRUE(a.left_empty());
                TEST_FALSE(a.right_empty());
                TEST_TRUE(la.empty());
                TEST_FALSE(ra.empty());
            }

            {// clear original right
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.left_emplace_back(3);
                a.right_emplace_back(4); 
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 
    
                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;
    
                a.right_clear();
    
                TEST_FALSE(a.left_empty());
                TEST_TRUE(a.right_empty());
                TEST_FALSE(la.empty());
                TEST_TRUE(ra.empty());
            }
            
            {// clear reference left
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.left_emplace_back(3);
                a.right_emplace_back(4); 
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 
    
                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;
    
                la.clear();
    
                TEST_TRUE(a.left_empty());
                TEST_FALSE(a.right_empty());
                TEST_TRUE(la.empty());
                TEST_FALSE(ra.empty());
            }

            {// clear reference right
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.left_emplace_back(3);
                a.right_emplace_back(4); 
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 
    
                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;
    
                ra.clear();
    
                TEST_FALSE(a.left_empty());
                TEST_TRUE(a.right_empty());
                TEST_FALSE(la.empty());
                TEST_TRUE(ra.empty());
            }

            {// clear original all
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.left_emplace_back(3);
                a.right_emplace_back(4); 
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 
    
                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;
    
                a.clear();
    
                TEST_TRUE(a.left_empty());
                TEST_TRUE(a.right_empty());
                TEST_TRUE(la.empty());
                TEST_TRUE(ra.empty());
            }
            embed::cout << "  finished: " << __func__ << embed::endl;
        }

        void pop(){
            {// pop left original
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 

                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;

                a.left_pop_back();

                TEST_FALSE(a.left_empty());
                TEST_FALSE(a.right_empty());
                TEST_EQUAL(a.left_size(), 1);
                TEST_EQUAL(a.right_size(), 2);

                TEST_FALSE(la.empty());
                TEST_FALSE(ra.empty());
                TEST_EQUAL(la.size(), 1);
                TEST_EQUAL(ra.size(), 2);

                a.left_pop_back();

                TEST_TRUE(a.left_empty());
                TEST_FALSE(a.right_empty());
                TEST_EQUAL(a.left_size(), 0);
                TEST_EQUAL(a.right_size(), 2);

                TEST_TRUE(la.empty());
                TEST_FALSE(ra.empty());
                TEST_EQUAL(la.size(), 0);
                TEST_EQUAL(ra.size(), 2);
            }
            {// pop right original
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 

                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;

                a.right_pop_back();

                TEST_FALSE(a.right_empty());
                TEST_FALSE(a.left_empty());
                TEST_EQUAL(a.right_size(), 1);
                TEST_EQUAL(a.left_size(), 2);

                TEST_FALSE(ra.empty());
                TEST_FALSE(la.empty());
                TEST_EQUAL(ra.size(), 1);
                TEST_EQUAL(la.size(), 2);

                a.right_pop_back();

                TEST_TRUE(a.right_empty());
                TEST_FALSE(a.left_empty());
                TEST_EQUAL(a.right_size(), 0);
                TEST_EQUAL(a.left_size(), 2);

                TEST_TRUE(ra.empty());
                TEST_FALSE(la.empty());
                TEST_EQUAL(ra.size(), 0);
                TEST_EQUAL(la.size(), 2);
            }
            {// pop left reference
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 

                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;

                la.pop_back();

                TEST_FALSE(a.left_empty());
                TEST_FALSE(a.right_empty());
                TEST_EQUAL(a.left_size(), 1);
                TEST_EQUAL(a.right_size(), 2);

                TEST_FALSE(la.empty());
                TEST_FALSE(ra.empty());
                TEST_EQUAL(la.size(), 1);
                TEST_EQUAL(ra.size(), 2);

                la.pop_back();

                TEST_TRUE(a.left_empty());
                TEST_FALSE(a.right_empty());
                TEST_EQUAL(a.left_size(), 0);
                TEST_EQUAL(a.right_size(), 2);

                TEST_TRUE(la.empty());
                TEST_FALSE(ra.empty());
                TEST_EQUAL(la.size(), 0);
                TEST_EQUAL(ra.size(), 2);
            }
            {// pop right reference
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 

                LeftDualArrayListRef la = a;
                RightDualArrayListRef ra = a;

                ra.pop_back();

                TEST_FALSE(a.right_empty());
                TEST_FALSE(a.left_empty());
                TEST_EQUAL(a.right_size(), 1);
                TEST_EQUAL(a.left_size(), 2);

                TEST_FALSE(ra.empty());
                TEST_FALSE(la.empty());
                TEST_EQUAL(ra.size(), 1);
                TEST_EQUAL(la.size(), 2);

                ra.pop_back();

                TEST_TRUE(a.right_empty());
                TEST_FALSE(a.left_empty());
                TEST_EQUAL(a.right_size(), 0);
                TEST_EQUAL(a.left_size(), 2);

                TEST_TRUE(ra.empty());
                TEST_FALSE(la.empty());
                TEST_EQUAL(ra.size(), 0);
                TEST_EQUAL(la.size(), 2);
            }
            embed::cout << "  finished: " << __func__ << embed::endl;
        }
    } // private namespace
     
    void DualArrayList_test(){
        embed::cout << "starting: " << __func__ << '{' << embed::endl;

        construction();
        emplace_back();
        clear();
        pop();

        embed::cout << '}' << embed::endl;
    }
}