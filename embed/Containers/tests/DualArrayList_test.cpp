#include "DualArrayList_test.hpp"

#include <embed/Containers/DualArrayList.hpp>
#include <embed/OStream/OStream.hpp>
#include <embed/TestFramework/TestFramework.hpp>

namespace embed{

    namespace{
        embed::TestResult construction(){
            TEST_START;

            DualArrayList<int, 5> a;

            LeftDualArrayList la = a;
            RightDualArrayList ra = a;

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

            TEST_END;
        }

        embed::TestResult emplace_back(){
            TEST_START;

            DualArrayList<int, 4> a;
            LeftDualArrayList la = a;
            RightDualArrayList ra = a;

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

            TEST_END;
        }

        embed::TestResult clear(){
            TEST_START;

            {// clear original left
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.left_emplace_back(3);
                a.right_emplace_back(4); 
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 
    
                LeftDualArrayList la = a;
                RightDualArrayList ra = a;
    
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
    
                LeftDualArrayList la = a;
                RightDualArrayList ra = a;
    
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
    
                LeftDualArrayList la = a;
                RightDualArrayList ra = a;
    
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
    
                LeftDualArrayList la = a;
                RightDualArrayList ra = a;
    
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
    
                LeftDualArrayList la = a;
                RightDualArrayList ra = a;
    
                a.clear();
    
                TEST_TRUE(a.left_empty());
                TEST_TRUE(a.right_empty());
                TEST_TRUE(la.empty());
                TEST_TRUE(ra.empty());
            }
            TEST_END;
        }

        embed::TestResult pop(){
            TEST_START;

            {// pop left original
                DualArrayList<int, 10> a;

                a.left_emplace_back(1);
                a.left_emplace_back(2);
                a.right_emplace_back(5); 
                a.right_emplace_back(6); 

                LeftDualArrayList la = a;
                RightDualArrayList ra = a;

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

                LeftDualArrayList la = a;
                RightDualArrayList ra = a;

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

                LeftDualArrayList la = a;
                RightDualArrayList ra = a;

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

                LeftDualArrayList la = a;
                RightDualArrayList ra = a;

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
            TEST_END;
        }

        embed::TestResult insert_value(){
            TEST_START;

            DualArrayList<int, 100> a;

            // insert single value into empty list

            a.left_insert(a.left_end(), 1);

            TEST_EQUAL(a.left_size(), 1);
            TEST_EQUAL(a.right_size(), 0);
            TEST_EQUAL(a.left_front(), 1);
            TEST_EQUAL(a.left_back(), 1);

            a.right_insert(a.right_begin(), 101);

            TEST_EQUAL(a.left_size(), 1);
            TEST_EQUAL(a.right_size(), 1);
            TEST_EQUAL(a.left_front(), 1);
            TEST_EQUAL(a.left_back(), 1);
            TEST_EQUAL(a.right_front(), 101);
            TEST_EQUAL(a.right_back(), 101);

            // insert single value into front

            a.left_insert(/* position */ 0, /* value */ 2);

            TEST_EQUAL(a.left_size(), 2);
            TEST_EQUAL(a.right_size(), 1);
            TEST_EQUAL(a.left_at(0), 2);
            TEST_EQUAL(a.left_at(1), 1);
            TEST_EQUAL(a.right_at(0), 101);

            a.right_insert(/* position */ a.right_begin(), /* value */ 102);

            TEST_EQUAL(a.left_size(), 2);
            TEST_EQUAL(a.right_size(), 2);
            TEST_EQUAL(a.left_at(0), 2);
            TEST_EQUAL(a.left_at(1), 1);
            TEST_EQUAL(a.right_at(0), 102);
            TEST_EQUAL(a.right_at(1), 101);

            // insert single value into back

            a.left_insert(/* position */ a.left_end(), /* value */ 3);

            TEST_EQUAL(a.left_size(), 3);
            TEST_EQUAL(a.right_size(), 2);
            TEST_EQUAL(a.left_at(0), 2);
            TEST_EQUAL(a.left_at(1), 1);
            TEST_EQUAL(a.left_at(2), 3);
            TEST_EQUAL(a.right_at(0), 102);
            TEST_EQUAL(a.right_at(1), 101);

            a.right_insert(/* position */ -1, /* value */ 103);

            TEST_EQUAL(a.left_size(), 3);
            TEST_EQUAL(a.right_size(), 3);
            TEST_EQUAL(a.left_at(0), 2);
            TEST_EQUAL(a.left_at(1), 1);
            TEST_EQUAL(a.left_at(2), 3);
            TEST_EQUAL(a.right_at(0), 102);
            TEST_EQUAL(a.right_at(1), 101);
            TEST_EQUAL(a.right_at(2), 103);

            // insert values into the middle

            a.left_insert(/* position */ 1, /* value */ 4);

            TEST_EQUAL(a.left_size(), 4);
            TEST_EQUAL(a.right_size(), 3);
            TEST_EQUAL(a.left_at(0), 2);
            TEST_EQUAL(a.left_at(1), 4);
            TEST_EQUAL(a.left_at(2), 1);
            TEST_EQUAL(a.left_at(3), 3);
            TEST_EQUAL(a.right_at(0), 102);
            TEST_EQUAL(a.right_at(1), 101);
            TEST_EQUAL(a.right_at(2), 103);

            a.right_insert(/* position */ a.right_begin()+1, /* value */ 104);

            TEST_EQUAL(a.left_size(), 4);
            TEST_EQUAL(a.right_size(), 4);
            TEST_EQUAL(a.left_at(0), 2);
            TEST_EQUAL(a.left_at(1), 4);
            TEST_EQUAL(a.left_at(2), 1);
            TEST_EQUAL(a.left_at(3), 3);
            TEST_EQUAL(a.right_at(0), 102);
            TEST_EQUAL(a.right_at(1), 104);
            TEST_EQUAL(a.right_at(2), 101);
            TEST_EQUAL(a.right_at(3), 103);

            TEST_END;
        }

        embed::TestResult insert_value_by_proxy(){
            TEST_START;

            DualArrayList<int, 100> a;
            LeftDualArrayList l = a;
            RightDualArrayList r = a;
            // insert single value into empty list

            l.insert(l.end(), 1);

            TEST_EQUAL(l.size(), 1);
            TEST_EQUAL(r.size(), 0);
            TEST_EQUAL(l.front(), 1);
            TEST_EQUAL(l.back(), 1);

            r.insert(r.begin(), 101);

            TEST_EQUAL(l.size(), 1);
            TEST_EQUAL(r.size(), 1);
            TEST_EQUAL(l.front(), 1);
            TEST_EQUAL(l.back(), 1);
            TEST_EQUAL(r.front(), 101);
            TEST_EQUAL(r.back(), 101);

            // insert single value into front

            l.insert(/* position */ 0, /* value */ 2);

            TEST_EQUAL(l.size(), 2);
            TEST_EQUAL(r.size(), 1);
            TEST_EQUAL(l.at(0), 2);
            TEST_EQUAL(l.at(1), 1);
            TEST_EQUAL(r.at(0), 101);

            r.insert(/* position */ r.begin(), /* value */ 102);

            TEST_EQUAL(l.size(), 2);
            TEST_EQUAL(r.size(), 2);
            TEST_EQUAL(l.at(0), 2);
            TEST_EQUAL(l.at(1), 1);
            TEST_EQUAL(r.at(0), 102);
            TEST_EQUAL(r.at(1), 101);

            // insert single value into back

            l.insert(/* position */ l.end(), /* value */ 3);

            TEST_EQUAL(l.size(), 3);
            TEST_EQUAL(r.size(), 2);
            TEST_EQUAL(l.at(0), 2);
            TEST_EQUAL(l.at(1), 1);
            TEST_EQUAL(l.at(2), 3);
            TEST_EQUAL(r.at(0), 102);
            TEST_EQUAL(r.at(1), 101);

            r.insert(/* position */ -1, /* value */ 103);

            TEST_EQUAL(l.size(), 3);
            TEST_EQUAL(r.size(), 3);
            TEST_EQUAL(l.at(0), 2);
            TEST_EQUAL(l.at(1), 1);
            TEST_EQUAL(l.at(2), 3);
            TEST_EQUAL(r.at(0), 102);
            TEST_EQUAL(r.at(1), 101);
            TEST_EQUAL(r.at(2), 103);

            // insert values into the middle

            l.insert(/* position */ 1, /* value */ 4);

            TEST_EQUAL(l.size(), 4);
            TEST_EQUAL(r.size(), 3);
            TEST_EQUAL(l.at(0), 2);
            TEST_EQUAL(l.at(1), 4);
            TEST_EQUAL(l.at(2), 1);
            TEST_EQUAL(l.at(3), 3);
            TEST_EQUAL(r.at(0), 102);
            TEST_EQUAL(r.at(1), 101);
            TEST_EQUAL(r.at(2), 103);

            r.insert(/* position */ r.begin()+1, /* value */ 104);

            TEST_EQUAL(l.size(), 4);
            TEST_EQUAL(r.size(), 4);
            TEST_EQUAL(l.at(0), 2);
            TEST_EQUAL(l.at(1), 4);
            TEST_EQUAL(l.at(2), 1);
            TEST_EQUAL(l.at(3), 3);
            TEST_EQUAL(r.at(0), 102);
            TEST_EQUAL(r.at(1), 104);
            TEST_EQUAL(r.at(2), 101);
            TEST_EQUAL(r.at(3), 103);

            TEST_END;
        }

        embed::TestResult insert_range(){
            TEST_START;

            DualArrayList<uint16_t, 100> a;

            // insert into empty list
            
            a.left_insert(/* position */0, /* values */ {1, 2});
            
            TEST_EQUAL(a.left_size(), 2);
            TEST_EQUAL(a.left_at(0), 1);
            TEST_EQUAL(a.left_at(1), 2);

            TEST_EQUAL(a.right_size(), 0);

            a.right_insert(a.right_begin(), {101, 102});
            
            TEST_EQUAL(a.left_size(), 2);
            TEST_EQUAL(a.left_at(0), 1);
            TEST_EQUAL(a.left_at(1), 2);

            TEST_EQUAL(a.right_size(), 2);
            TEST_EQUAL(a.right_at(0), 101);
            TEST_EQUAL(a.right_at(1), 102);

            // insert into front
            
            a.left_insert(/* position */0, /* values */ {3, 4});
            
            TEST_EQUAL(a.left_size(), 4);
            TEST_EQUAL(a.left_at(0), 3);
            TEST_EQUAL(a.left_at(1), 4);
            TEST_EQUAL(a.left_at(2), 1);
            TEST_EQUAL(a.left_at(3), 2);

            TEST_EQUAL(a.right_size(), 2);
            TEST_EQUAL(a.right_at(0), 101);
            TEST_EQUAL(a.right_at(1), 102);

            a.right_insert(a.right_begin(), {103, 104});

            TEST_EQUAL(a.left_size(), 4);
            TEST_EQUAL(a.left_at(0), 3);
            TEST_EQUAL(a.left_at(1), 4);
            TEST_EQUAL(a.left_at(2), 1);
            TEST_EQUAL(a.left_at(3), 2);

            TEST_EQUAL(a.right_size(), 4);
            TEST_EQUAL(a.right_at(0), 103);
            TEST_EQUAL(a.right_at(1), 104);
            TEST_EQUAL(a.right_at(2), 101);
            TEST_EQUAL(a.right_at(3), 102);

            // insert into back

            a.left_insert(/* position */-1, /* values */ {5, 6});

            TEST_EQUAL(a.left_size(), 6);
            TEST_EQUAL(a.left_at(0), 3);
            TEST_EQUAL(a.left_at(1), 4);
            TEST_EQUAL(a.left_at(2), 1);
            TEST_EQUAL(a.left_at(3), 2);
            TEST_EQUAL(a.left_at(4), 5);
            TEST_EQUAL(a.left_at(5), 6);

            TEST_EQUAL(a.right_size(), 4);
            TEST_EQUAL(a.right_at(0), 103);
            TEST_EQUAL(a.right_at(1), 104);
            TEST_EQUAL(a.right_at(2), 101);
            TEST_EQUAL(a.right_at(3), 102);

            a.right_insert(a.right_end(), {105, 106});

            TEST_EQUAL(a.left_size(), 6);
            TEST_EQUAL(a.left_at(0), 3);
            TEST_EQUAL(a.left_at(1), 4);
            TEST_EQUAL(a.left_at(2), 1);
            TEST_EQUAL(a.left_at(3), 2);
            TEST_EQUAL(a.left_at(4), 5);
            TEST_EQUAL(a.left_at(5), 6);

            TEST_EQUAL(a.right_size(), 6);
            TEST_EQUAL(a.right_at(0), 103);
            TEST_EQUAL(a.right_at(1), 104);
            TEST_EQUAL(a.right_at(2), 101);
            TEST_EQUAL(a.right_at(3), 102);
            TEST_EQUAL(a.right_at(4), 105);
            TEST_EQUAL(a.right_at(5), 106);

            // insert middle

            a.left_insert(/* position */3, /* values */ {7, 8});

            TEST_EQUAL(a.left_size(), 8);
            TEST_EQUAL(a.left_at(0), 3);
            TEST_EQUAL(a.left_at(1), 4);
            TEST_EQUAL(a.left_at(2), 1);
            TEST_EQUAL(a.left_at(3), 7);
            TEST_EQUAL(a.left_at(4), 8);
            TEST_EQUAL(a.left_at(5), 2);
            TEST_EQUAL(a.left_at(6), 5);
            TEST_EQUAL(a.left_at(7), 6);

            TEST_EQUAL(a.right_size(), 6);
            TEST_EQUAL(a.right_at(0), 103);
            TEST_EQUAL(a.right_at(1), 104);
            TEST_EQUAL(a.right_at(2), 101);
            TEST_EQUAL(a.right_at(3), 102);
            TEST_EQUAL(a.right_at(4), 105);
            TEST_EQUAL(a.right_at(5), 106);

            a.right_insert(a.right_begin() + 3, {107, 108});

            TEST_EQUAL(a.left_size(), 8);
            TEST_EQUAL(a.left_at(0), 3);
            TEST_EQUAL(a.left_at(1), 4);
            TEST_EQUAL(a.left_at(2), 1);
            TEST_EQUAL(a.left_at(3), 7);
            TEST_EQUAL(a.left_at(4), 8);
            TEST_EQUAL(a.left_at(5), 2);
            TEST_EQUAL(a.left_at(6), 5);
            TEST_EQUAL(a.left_at(7), 6);

            TEST_EQUAL(a.right_size(), 8);
            TEST_EQUAL(a.right_at(0), 103);
            TEST_EQUAL(a.right_at(1), 104);
            TEST_EQUAL(a.right_at(2), 101);
            TEST_EQUAL(a.right_at(3), 107);
            TEST_EQUAL(a.right_at(4), 108);
            TEST_EQUAL(a.right_at(5), 102);
            TEST_EQUAL(a.right_at(6), 105);
            TEST_EQUAL(a.right_at(7), 106);

            TEST_END;
        }

        embed::TestResult insert_range_by_proxy(){
            TEST_START;

            DualArrayList<uint16_t, 100> a;
            LeftDualArrayList l = a;
            RightDualArrayList r = a;
            // insert into empty list
            
            l.insert(/* position */0, /* values */ {1, 2});
            
            TEST_EQUAL(l.size(), 2);
            TEST_EQUAL(l.at(0), 1);
            TEST_EQUAL(l.at(1), 2);

            TEST_EQUAL(r.size(), 0);

            r.insert(r.begin(), {101, 102});
            
            TEST_EQUAL(l.size(), 2);
            TEST_EQUAL(l.at(0), 1);
            TEST_EQUAL(l.at(1), 2);

            TEST_EQUAL(r.size(), 2);
            TEST_EQUAL(r.at(0), 101);
            TEST_EQUAL(r.at(1), 102);

            // insert into front
            
            l.insert(/* position */0, /* values */ {3, 4});
            
            TEST_EQUAL(l.size(), 4);
            TEST_EQUAL(l.at(0), 3);
            TEST_EQUAL(l.at(1), 4);
            TEST_EQUAL(l.at(2), 1);
            TEST_EQUAL(l.at(3), 2);

            TEST_EQUAL(r.size(), 2);
            TEST_EQUAL(r.at(0), 101);
            TEST_EQUAL(r.at(1), 102);

            r.insert(r.begin(), {103, 104});

            TEST_EQUAL(l.size(), 4);
            TEST_EQUAL(l.at(0), 3);
            TEST_EQUAL(l.at(1), 4);
            TEST_EQUAL(l.at(2), 1);
            TEST_EQUAL(l.at(3), 2);

            TEST_EQUAL(r.size(), 4);
            TEST_EQUAL(r.at(0), 103);
            TEST_EQUAL(r.at(1), 104);
            TEST_EQUAL(r.at(2), 101);
            TEST_EQUAL(r.at(3), 102);

            // insert into back

            l.insert(/* position */-1, /* values */ {5, 6});

            TEST_EQUAL(l.size(), 6);
            TEST_EQUAL(l.at(0), 3);
            TEST_EQUAL(l.at(1), 4);
            TEST_EQUAL(l.at(2), 1);
            TEST_EQUAL(l.at(3), 2);
            TEST_EQUAL(l.at(4), 5);
            TEST_EQUAL(l.at(5), 6);

            TEST_EQUAL(r.size(), 4);
            TEST_EQUAL(r.at(0), 103);
            TEST_EQUAL(r.at(1), 104);
            TEST_EQUAL(r.at(2), 101);
            TEST_EQUAL(r.at(3), 102);

            r.insert(r.end(), {105, 106});

            TEST_EQUAL(l.size(), 6);
            TEST_EQUAL(l.at(0), 3);
            TEST_EQUAL(l.at(1), 4);
            TEST_EQUAL(l.at(2), 1);
            TEST_EQUAL(l.at(3), 2);
            TEST_EQUAL(l.at(4), 5);
            TEST_EQUAL(l.at(5), 6);

            TEST_EQUAL(r.size(), 6);
            TEST_EQUAL(r.at(0), 103);
            TEST_EQUAL(r.at(1), 104);
            TEST_EQUAL(r.at(2), 101);
            TEST_EQUAL(r.at(3), 102);
            TEST_EQUAL(r.at(4), 105);
            TEST_EQUAL(r.at(5), 106);

            // insert middle

            l.insert(/* position */3, /* values */ {7, 8});

            TEST_EQUAL(l.size(), 8);
            TEST_EQUAL(l.at(0), 3);
            TEST_EQUAL(l.at(1), 4);
            TEST_EQUAL(l.at(2), 1);
            TEST_EQUAL(l.at(3), 7);
            TEST_EQUAL(l.at(4), 8);
            TEST_EQUAL(l.at(5), 2);
            TEST_EQUAL(l.at(6), 5);
            TEST_EQUAL(l.at(7), 6);

            TEST_EQUAL(r.size(), 6);
            TEST_EQUAL(r.at(0), 103);
            TEST_EQUAL(r.at(1), 104);
            TEST_EQUAL(r.at(2), 101);
            TEST_EQUAL(r.at(3), 102);
            TEST_EQUAL(r.at(4), 105);
            TEST_EQUAL(r.at(5), 106);

            r.insert(r.begin() + 3, {107, 108});

            TEST_EQUAL(l.size(), 8);
            TEST_EQUAL(l.at(0), 3);
            TEST_EQUAL(l.at(1), 4);
            TEST_EQUAL(l.at(2), 1);
            TEST_EQUAL(l.at(3), 7);
            TEST_EQUAL(l.at(4), 8);
            TEST_EQUAL(l.at(5), 2);
            TEST_EQUAL(l.at(6), 5);
            TEST_EQUAL(l.at(7), 6);

            TEST_EQUAL(r.size(), 8);
            TEST_EQUAL(r.at(0), 103);
            TEST_EQUAL(r.at(1), 104);
            TEST_EQUAL(r.at(2), 101);
            TEST_EQUAL(r.at(3), 107);
            TEST_EQUAL(r.at(4), 108);
            TEST_EQUAL(r.at(5), 102);
            TEST_EQUAL(r.at(6), 105);
            TEST_EQUAL(r.at(7), 106);

            TEST_END;
        }

        embed::TestResult erase(){
            TEST_START;

            DualArrayList<int, 100> a;

            a.left_assign({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
            a.right_assign({10, 11, 12, 13, 14, 15, 16, 17, 18, 19});

            // erase front

            
            a.left_erase(0);
            //{1, 2, 3, 4, 5, 6, 7, 8, 9}

            TEST_EQUAL(a.left_size(), 9);
            TEST_EQUAL(a.left_front(), 1);
            TEST_EQUAL(a.right_size(), 10);
            
            a.right_erase(a.right_begin());
            //{11, 12, 13, 14, 15, 16, 17, 18, 19}

            TEST_EQUAL(a.left_size(), 9);
            TEST_EQUAL(a.right_size(), 9);
            TEST_EQUAL(a.right_front(), 11);

            // erase end

            a.left_erase(a.left_end()-1);
            //{1, 2, 3, 4, 5, 6, 7, 8}

            TEST_EQUAL(a.left_size(), 8);
            TEST_EQUAL(a.left_back(), 8);
            TEST_EQUAL(a.right_size(), 9);

            a.right_erase(-1);
            //{11, 12, 13, 14, 15, 16, 17, 18}

            TEST_EQUAL(a.left_size(), 8);
            TEST_EQUAL(a.right_size(), 8);
            TEST_EQUAL(a.right_back(), 18);

            // erase middle

            a.left_erase(3);
            //{1, 2, 3, 5, 6, 7, 8}
            TEST_EQUAL(a.left_size(), 7);
            TEST_EQUAL(a.left_at(3), 5);

            a.right_erase(a.right_begin() + 3);
            //{11, 12, 13, 15, 16, 17, 18}
            TEST_EQUAL(a.right_size(), 7);
            TEST_EQUAL(a.right_at(3), 15);
            
            // erase range

            a.left_erase(1, 5);
            //{1, 7, 8}

            TEST_EQUAL(a.left_size(), 3);
            TEST_EQUAL(a.left_at(0), 1);
            TEST_EQUAL(a.left_at(1), 7);
            TEST_EQUAL(a.left_at(2), 8);

            a.right_erase(a.right_begin() + 1, a.right_begin() + 5);
            //{11, 17, 18}

            TEST_EQUAL(a.right_size(), 3);
            TEST_EQUAL(a.right_at(0), 11);
            TEST_EQUAL(a.right_at(1), 17);
            TEST_EQUAL(a.right_at(2), 18);

            TEST_END;
        }

    } // private namespace
     
    embed::TestResult DualArrayList_test(){
        TEST_GROUP;

        return embed::TestResult()
            | construction
            | emplace_back
            | clear
            | pop
            | insert_value
            | insert_value_by_proxy
            | insert_range
            | insert_range_by_proxy
            | erase
            ;
    }
}