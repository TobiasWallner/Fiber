
#include "embed/Containers/StaticArrayList.hpp"
#include <embed/test/test.hpp>

void embed::StaticArrayList_test(){
    embed::cout << "start: StaticArrayList_test()" << embed::endl; 

    using namespace embed;

    {

        StaticArrayList<int, 3> arr;
        const StaticArrayList<int, 3>& const_ref_arr = arr;

        // check size and empty after creation
        TEST_EQUAL(arr.size(), 0);
        TEST_EQUAL(arr.capacity(), 3);
        TEST_EQUAL(arr.max_size(), 3);
        TEST_TRUE(arr.empty());
        TEST_EQUAL(arr.begin(), arr.end());
        TEST_EQUAL(const_ref_arr.begin(), const_ref_arr.end());
        TEST_EQUAL(arr.cbegin(), arr.cend());

        // check after 1 insertion
        arr.emplace_back(3);
        TEST_FALSE(arr.empty());
        TEST_EQUAL(arr.size(), 1);
        TEST_NOT_EQUAL(arr.begin(), arr.end());
        TEST_NOT_EQUAL(const_ref_arr.begin(), const_ref_arr.end());
        TEST_NOT_EQUAL(arr.cbegin(), arr.cend());
        TEST_EQUAL(*arr.begin(), 3);
        TEST_EQUAL(arr.front(), 3);
        TEST_EQUAL(arr.back(), 3);
        TEST_EQUAL(arr.front(), arr.back());

        // check after 2nd insertion
        arr.emplace_back(5);
        TEST_FALSE(arr.empty());
        TEST_EQUAL(arr.size(), 2);
        TEST_NOT_EQUAL(arr.begin(), arr.end());
        TEST_NOT_EQUAL(const_ref_arr.begin(), const_ref_arr.end());
        TEST_NOT_EQUAL(arr.cbegin(), arr.cend());
        TEST_EQUAL(*arr.begin(), 3);
        TEST_EQUAL(arr.front(), 3);
        TEST_EQUAL(arr.back(), 5);
        TEST_NOT_EQUAL(arr.front(), arr.back());

        // check after 3rd insertion
        arr.emplace_back(42);
        TEST_FALSE(arr.empty());
        TEST_EQUAL(arr.size(), 3);
        TEST_NOT_EQUAL(arr.begin(), arr.end());
        TEST_NOT_EQUAL(const_ref_arr.begin(), const_ref_arr.end());
        TEST_NOT_EQUAL(arr.cbegin(), arr.cend());
        TEST_EQUAL(*arr.begin(), 3);
        TEST_EQUAL(arr.front(), 3);
        TEST_EQUAL(arr.back(), 42);
        TEST_NOT_EQUAL(arr.front(), arr.back());

        // check after 4th insertion out of size - allocation error
        try{
            arr.emplace_back(55); // should throw
            TEST_ERROR; // should not be reached
        }catch(const std::exception& e){}
        // same checks as before 55 should not be added
        TEST_FALSE(arr.empty());
        TEST_EQUAL(arr.size(), 3);
        TEST_NOT_EQUAL(arr.begin(), arr.end());
        TEST_NOT_EQUAL(const_ref_arr.begin(), const_ref_arr.end());
        TEST_NOT_EQUAL(arr.cbegin(), arr.cend());
        TEST_EQUAL(*arr.begin(), 3);
        TEST_EQUAL(arr.front(), 3);
        TEST_EQUAL(arr.back(), 42);
        TEST_NOT_EQUAL(arr.front(), arr.back());

        // check after clear
        arr.clear();
        TEST_EQUAL(arr.size(), 0);
        TEST_TRUE(arr.empty());
        TEST_EQUAL(arr.begin(), arr.end());
        TEST_EQUAL(const_ref_arr.begin(), const_ref_arr.end());
        TEST_EQUAL(arr.cbegin(), arr.cend());
    }

    {
        StaticArrayList<int, 6> arr;
        arr.assign(/*count*/5, /*value*/188);
        TEST_EQUAL(arr.size(), 5);
        TEST_FALSE(arr.empty());
        TEST_EQUAL(arr.front(), 188);
        TEST_EQUAL(arr.back(), 188);

        // reassign clears and not appends
        arr.assign(/*count*/4, /*value*/99);
        TEST_EQUAL(arr.size(), 4);
        TEST_FALSE(arr.empty());
        TEST_EQUAL(arr.front(), 99);
        TEST_EQUAL(arr.back(), 99);

        // reassign clears and not appends
        arr.assign({1, 2, 3, 4});
        TEST_EQUAL(arr.size(), 4);
        TEST_EQUAL(arr[0], 1);
        TEST_EQUAL(arr[1], 2);
        TEST_EQUAL(arr[2], 3);
        TEST_EQUAL(arr[3], 4);
        TEST_EQUAL(arr[-1], 4);
        TEST_EQUAL(arr[-2], 3);
        TEST_EQUAL(arr[-3], 2);
        TEST_EQUAL(arr[-4], 1);
        TEST_FALSE(arr.empty());
        TEST_EQUAL(arr.front(), 1);
        TEST_EQUAL(arr.back(), 4);

        arr.insert(/* pos */2, /* value */ 11);
        TEST_EQUAL(arr.size(), 5);
        TEST_EQUAL(arr[0], 1);
        TEST_EQUAL(arr[1], 2);
        TEST_EQUAL(arr[2], 11);
        TEST_EQUAL(arr[3], 3);
        TEST_EQUAL(arr[4], 4);

        arr.insert(/* pos */arr.begin()+1, /* value */ 88);
        TEST_EQUAL(arr.size(), 6);
        TEST_EQUAL(arr[0], 1);
        TEST_EQUAL(arr[1], 88);
        TEST_EQUAL(arr[2], 2);
        TEST_EQUAL(arr[3], 11);
        TEST_EQUAL(arr[4], 3);
        TEST_EQUAL(arr[5], 4);
        
        try{
            arr.insert(/* pos */arr.begin()+1, /* value */ 88);
            TEST_ERROR;
        }catch(std::exception& e){

        }

        arr.clear();
        arr.insert(0, 456);
        TEST_EQUAL(arr.size(), 1);
        TEST_FALSE(arr.empty());
        TEST_EQUAL(arr.front(), 456);
    }
    {

        StaticArrayList<unsigned int, 10> a({1, 2, 3, 4});
        TEST_EQUAL(a.size(), 4);
        TEST_EQUAL(a[0], 1);
        TEST_EQUAL(a[1], 2);
        TEST_EQUAL(a[2], 3);
        TEST_EQUAL(a[3], 4);

        StaticArrayList<int, 11> b({11, 22, 33, 44});
        TEST_EQUAL(b.size(), 4);
        TEST_EQUAL(b[0], 11);
        TEST_EQUAL(b[1], 22);
        TEST_EQUAL(b[2], 33);
        TEST_EQUAL(b[3], 44);

        StaticArrayList<long, 9> c(a);
        TEST_EQUAL(c.size(), 4);
        c.append(b);
        TEST_EQUAL(c.size(), 8);
        TEST_EQUAL(c[0], 1);
        TEST_EQUAL(c[1], 2);
        TEST_EQUAL(c[2], 3);
        TEST_EQUAL(c[3], 4);
        TEST_EQUAL(c[0+4], 11);
        TEST_EQUAL(c[1+4], 22);
        TEST_EQUAL(c[2+4], 33);
        TEST_EQUAL(c[3+4], 44);

        c = a;
        c.append(b);
        TEST_EQUAL(c.size(), 8);
        TEST_EQUAL(c[0], 1);
        TEST_EQUAL(c[1], 2);
        TEST_EQUAL(c[2], 3);
        TEST_EQUAL(c[3], 4);
        TEST_EQUAL(c[0+4], 11);
        TEST_EQUAL(c[1+4], 22);
        TEST_EQUAL(c[2+4], 33);
        TEST_EQUAL(c[3+4], 44);

        c = a;
        c.insert(2, b);
        TEST_EQUAL(c.size(), 8);
        TEST_EQUAL(c[0], 1);
        TEST_EQUAL(c[1], 2);
        TEST_EQUAL(c[2], 11);
        TEST_EQUAL(c[3], 22);
        TEST_EQUAL(c[4], 33);
        TEST_EQUAL(c[5], 44);
        TEST_EQUAL(c[6], 3);
        TEST_EQUAL(c[7], 4);

        try{
            c.insert(3, b);
            TEST_ERROR;
        }catch(std::exception& e){}
        TEST_EQUAL(c.size(), 8);
        TEST_EQUAL(c[0], 1);
        TEST_EQUAL(c[1], 2);
        TEST_EQUAL(c[2], 11);
        TEST_EQUAL(c[3], 22);
        TEST_EQUAL(c[4], 33);
        TEST_EQUAL(c[5], 44);
        TEST_EQUAL(c[6], 3);
        TEST_EQUAL(c[7], 4);

        auto r = c.erase(0);
        TEST_EQUAL(r, c.begin()+0);
        TEST_EQUAL(c.size(), 7);
        TEST_EQUAL(c[0], 2);
        TEST_EQUAL(c[1], 11);
        TEST_EQUAL(c[2], 22);
        TEST_EQUAL(c[3], 33);
        TEST_EQUAL(c[4], 44);
        TEST_EQUAL(c[5], 3);
        TEST_EQUAL(c[6], 4);

        r = c.erase(3);
        TEST_EQUAL(r, c.begin()+3);
        TEST_EQUAL(c.size(), 6);
        TEST_EQUAL(c[0], 2);
        TEST_EQUAL(c[1], 11);
        TEST_EQUAL(c[2], 22);
        TEST_EQUAL(c[3], 44);
        TEST_EQUAL(c[4], 3);
        TEST_EQUAL(c[5], 4);
        
        r = c.erase(-1);
        TEST_EQUAL(r, c.end());
        TEST_EQUAL(c.size(), 5);
        TEST_EQUAL(c[0], 2);
        TEST_EQUAL(c[1], 11);
        TEST_EQUAL(c[2], 22);
        TEST_EQUAL(c[3], 44);
        TEST_EQUAL(c[4], 3);

        r = c.erase(1, 3);
        TEST_EQUAL(r, c.begin() + 1);
        TEST_EQUAL(c.size(), 3);
        TEST_EQUAL(c[0], 2);
        TEST_EQUAL(c[1], 44);
        TEST_EQUAL(c[2], 3);

        r = c.erase(1, 3);
        TEST_EQUAL(r, c.end());
        TEST_EQUAL(c.size(), 1);
        TEST_EQUAL(c[0], 2);

        c.pop_back();
        TEST_TRUE(c.empty());
        TEST_EQUAL(c.size(), 0);
    }

    {
        StaticArrayList<unsigned int, 10> a({1, 2, 3, 4});
        StaticArrayList<unsigned int, 10> b({1, 2, 3, 4, 5});
        StaticArrayList<unsigned int, 10> c({0, 1, 2, 3});
        StaticArrayList<unsigned int, 10> d({0, 1, 3, 3});

        TEST_TRUE(equal(a, a));
        TEST_FALSE(equal(a, b));
        TEST_FALSE(equal(a, c));

        TEST_FALSE(not_equal(a, a));
        TEST_TRUE(not_equal(a, b));
        TEST_TRUE(not_equal(a, c));

        TEST_TRUE(less(c, a))
        TEST_FALSE(less(a, c))
        TEST_TRUE(greater(a, c))
        TEST_FALSE(greater(c, a))

        TEST_TRUE(less_equal(d, a))
        TEST_FALSE(less_equal(a, d))
        TEST_TRUE(greater_equal(a, d))
        TEST_FALSE(greater_equal(d, a))
    }

    {
        StaticArrayList<unsigned int, 10> a({1, 2, 3, 4, 5, 6});
        StaticArrayList<unsigned int, 10> b({1, 2, 0, 10, 5, 11});
        StaticArrayList<bool, 10> expected_equal({true, true, false, false, true, false});

        TEST_FUNC2(equal, a == b, expected_equal);

        StaticArrayList<bool, 10> mask({true, true, false, false, true, false});
        StaticArrayList<int, 10> expected_masked({1, 2, 5});
        auto masked = a[mask];
        TEST_FUNC2(equal, masked, expected_masked);

        StaticArrayList<long, 10> indices({1, 3, 5, 2});
        StaticArrayList<int, 10> expected_indexed({2, 10, 11, 0});
        auto indexed = b[indices];
        TEST_FUNC2(equal, indexed, expected_indexed);
    }
    {
        StaticArrayList<bool, 5> a({true, true, false});
        StaticArrayList<bool, 5> b({true, true, true});
        StaticArrayList<bool, 5> c({false, false, false});

        TEST_TRUE(any(a));
        TEST_TRUE(any(b));
        TEST_FALSE(any(c));

        TEST_FALSE(all(a));
        TEST_TRUE(all(b));
        TEST_FALSE(all(c));

        TEST_FALSE(none(a));
        TEST_FALSE(none(b));
        TEST_TRUE(none(c));
    }

    {
        StaticArrayList<int, 5> a({1, 0, 10});
        StaticArrayList<bool, 5> expected_not_a({false, true, false});
        TEST_FUNC2(equal, !a, expected_not_a);
        
        StaticArrayList<int, 5> b({true, true, false});
        StaticArrayList<bool, 5> expected_not_b({false, false, true});
        TEST_FUNC2(equal, !b, expected_not_b);
    }

    {
        StaticArrayList<int, 6> a{0, 1, 2, 3, 4, 5};

        auto square_result = for_each(a, [](const int& i){return i*i;});
        StaticArrayList<int, 6> expected_square_result{0, 1, 4, 9, 16, 25};
        TEST_FUNC2(equal, square_result, expected_square_result);

        a.for_each([](const int& i){return i*i;});
        TEST_FUNC2(equal, a, expected_square_result);
    }
    embed::cout << "finished: StaticArrayList_test()" << embed::endl; 
}
