#include <gtest/gtest.h>
#include "span_list.h"

using namespace std;

TEST(span_list, insert_range) {
    {
        span_list sl;
        sl.insert_range(2, 6);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{2, 6}})
        );
    }
    {
        span_list sl;
        sl.insert_range(2, 6);
        sl.insert_range(8, 10);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{2, 6},
                              {8, 10}})
        );
    }
    {
        span_list sl;
        sl.insert_range(8, 10);
        sl.insert_range(2, 6);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{2, 6},
                              {8, 10}})
        );
    }
    {
        span_list sl;
        sl.insert_range(2, 6);
        sl.insert_range(7, 10);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{2, 10}})
        );
    }
    {
        span_list sl;
        sl.insert_range(7, 10);
        sl.insert_range(2, 6);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{2, 10}})
        );
    }
    {
        span_list sl;
        sl.insert_range(1, 5);
        sl.insert_range(11, 15);
        sl.insert_range(4, 12);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{1, 15}})
        );
    }
    {
        span_list sl;
        sl.insert_range(1, 5);
        sl.insert_range(11, 15);
        sl.insert_range(1, 15);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{1, 15}})
        );
    }
    {
        span_list sl;
        sl.insert_range(1, 5);
        sl.insert_range(11, 15);
        sl.insert_range(-5, 20);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{-5, 20}})
        );
    }
}

TEST(span_list, remove_range) {
    {
        span_list sl;
        sl.insert_range(1, 10);
        sl.remove_range(1, 10);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{})
        );
    }

    {
        span_list sl;
        sl.insert_range(1, 10);
        sl.insert_range(21, 30);
        sl.remove_range(11, 20);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{1,  10},
                              {21, 30}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 9);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{10, 20},
                              {30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 10);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{11, 20},
                              {30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 11);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{12, 20},
                              {30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 20);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 29);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 30);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{31, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 39);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{40, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 40);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(1, 45);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(10, 10);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{11, 20},
                              {30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(10, 19);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{20, 20},
                              {30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(10, 20);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(10, 29);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{30, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(10, 30);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{31, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(10, 39);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{40, 40}})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(10, 40);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(10, 45);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{})
        );
    }

    {
        span_list sl;
        sl.insert_range(10, 20);
        sl.insert_range(30, 40);
        sl.remove_range(15, 15);
        ASSERT_EQ(
                (vector<span>{sl.begin(), sl.end()}),
                (vector<span>{{10, 14},
                              {16, 20},
                              {30, 40}})
        );
    }
}

