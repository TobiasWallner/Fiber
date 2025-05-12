#include "Future_test.hpp"

#include <embed/Future/Future.hpp>
#include <embed/OStream/OStream.hpp>
#include <embed/TestFramework/TestFramework.hpp>

namespace embed
{

    static embed::TestResult make_future_promise_test(){
        TEST_START;

        auto [future, promise] = embed::make_future_promise<int>();

        TEST_TRUE(future.is_waiting());
        TEST_FALSE(future.is_ready());
        TEST_FALSE(future.is_broken_promise());
        TEST_FALSE(future.await_ready());
        TEST_FALSE(future.await_resume().has_value());
        TEST_TRUE(promise.is_connected_to(future));
        TEST_TRUE(future.is_connected_to(promise));

        TEST_END;
    }

    static embed::TestResult promise_set_value_test(){
        TEST_START;

        auto [future, promise] = embed::make_future_promise<int>();

        TEST_TRUE(future.is_waiting());
        TEST_FALSE(future.is_ready());
        TEST_FALSE(future.is_broken_promise());
        TEST_FALSE(future.await_ready());
        TEST_FALSE(future.await_resume().has_value());

        promise.set_value(5);

        TEST_FALSE(future.is_waiting());
        TEST_TRUE(future.is_ready());
        TEST_FALSE(future.is_broken_promise());
        TEST_TRUE(future.await_ready());
        TEST_TRUE(future.await_resume().has_value());
        TEST_EQUAL(future.get(), 5);
        TEST_EQUAL(future.await_resume().value(), 5);

        TEST_END;
    }

    static embed::TestResult promise_assign_value_test(){
        TEST_START;

        auto [future, promise] = embed::make_future_promise<int>();

        TEST_TRUE(future.is_waiting());
        TEST_FALSE(future.is_ready());
        TEST_FALSE(future.is_broken_promise());
        TEST_FALSE(future.await_ready());
        TEST_FALSE(future.await_resume().has_value());

        promise = 5;

        TEST_FALSE(future.is_waiting());
        TEST_TRUE(future.is_ready());
        TEST_FALSE(future.is_broken_promise());
        TEST_TRUE(future.await_ready());
        TEST_TRUE(future.await_resume().has_value());
        TEST_EQUAL(future.get(), 5);
        TEST_EQUAL(future.await_resume().value(), 5);

        TEST_END;
    }

    static embed::TestResult handover_test(){
        TEST_START;

        // volatile to prevent the compiler from folding variables
        volatile Future<int> fut;
        volatile Promise<int> pro;

        Future<int>& f = *const_cast<Future<int>*>(&fut);
        Promise<int>& p = *const_cast<Promise<int>*>(&pro);

        auto [future, promise] = embed::make_future_promise<int>();
        
        TEST_TRUE(future.is_connected_to(promise));
        TEST_FALSE(future.is_connected_to(p));
        TEST_TRUE(promise.is_connected_to(future));
        TEST_FALSE(promise.is_connected_to(f));
        TEST_FALSE(f.is_connected_to(promise));
        TEST_FALSE(f.is_connected_to(p));
        TEST_FALSE(p.is_connected_to(future));
        TEST_FALSE(p.is_connected_to(f));

        f = std::move(future);

        TEST_FALSE(future.is_connected_to(promise));
        TEST_FALSE(future.is_connected_to(p));
        TEST_FALSE(promise.is_connected_to(future));
        TEST_TRUE(promise.is_connected_to(f));
        TEST_TRUE(f.is_connected_to(promise));
        TEST_FALSE(f.is_connected_to(p));
        TEST_FALSE(p.is_connected_to(future));
        TEST_FALSE(p.is_connected_to(f));

        p = std::move(promise);
        
        TEST_FALSE(future.is_connected_to(promise));
        TEST_FALSE(future.is_connected_to(p));
        TEST_FALSE(promise.is_connected_to(future));
        TEST_FALSE(promise.is_connected_to(f));
        TEST_FALSE(f.is_connected_to(promise));
        TEST_TRUE(f.is_connected_to(p));
        TEST_FALSE(p.is_connected_to(future));
        TEST_TRUE(p.is_connected_to(f));

        TEST_END;
    }

    static embed::TestResult broken_promise_test(){
        TEST_START;

        embed::Future<int> f;
        TEST_TRUE(f.is_broken_promise());
        {
            auto [future, promise] = embed::make_future_promise<int>();

            TEST_FALSE(future.is_broken_promise());

            f = std::move(future);

            TEST_TRUE(future.is_broken_promise());
            TEST_FALSE(f.is_broken_promise());
        }// promise dies without setting value

        TEST_TRUE(f.is_broken_promise());
        TEST_THROW((void)f.get());
        TEST_EQUAL(f.get_if(), nullptr);

        TEST_END;
    }

    embed::TestResult Future_test(){
        TEST_GROUP;

        return TestResult()
            | make_future_promise_test
            | promise_set_value_test
            | promise_assign_value_test
            | handover_test
            | broken_promise_test;
    }
} // namespace embed
