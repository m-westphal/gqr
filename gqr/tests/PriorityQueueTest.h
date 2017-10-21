// -*- C++ -*-
#ifndef PRIORITY_QUEUE_TEST_H
#define PRIORITY_QUEUE_TEST_H

#include <stdlib.h>

#include "PriorityQueue.h"
#include "TestSuite.h"

class PriorityQueueTest:public CxxTest::TestSuite {
 private:
    PriorityQueue queue;
    size_t n;

 public:
    PriorityQueueTest() : n(200) { /* NOCODE */ }

    void testInsertKeyAuto() {
        for (int i = 1; i <= n; ++i)
            queue.insert(i, i * 10);

        TS_ASSERT_EQUALS(queue.getSize(), n);
    }

    void testDecreaseKeyAuto() {
        srand(0);
        for (int i = 1; i <= n; ++i) {
            size_t key = rand() % i * 10;
            queue.insert(i, key);
        }

        TS_ASSERT_EQUALS(queue.getSize(), n);
    };

    void testPopPeekAuto() {
        std::pair < size_t, size_t > prev = std::make_pair(0, 0);
        while (!queue.empty()) {
            std::pair < size_t, size_t > elm = queue.peekMin();
            queue.popMin();

            TS_ASSERT(elm.second >= prev.second);
            prev = elm;
        }

        TS_ASSERT_EQUALS(queue.getSize(), 0);
    };

    void testInsertManual() {
        queue.insert(1, 4);
        queue.insert(2, 2);
        queue.insert(6, 2);
        queue.insert(3, 18);
        queue.insert(8, 5);
        queue.insert(4, 12);
        queue.insert(5, 9);
        queue.insert(10, 1);
        queue.insert(7, 14);
        queue.insert(8, 3);
        queue.insert(9, 11);

        TS_ASSERT_EQUALS(queue.getSize(), 10);
    }

    void testDecreaseManual() {
        queue.insert(1, 3);
        queue.insert(2, 1);
        queue.insert(6, 0);
        queue.insert(3, 15);
        queue.insert(8, 2);
        queue.insert(4, 10);
        queue.insert(5, 7);
        queue.insert(10, 1);
        queue.insert(7, 11);
        queue.insert(8, 2);
        queue.insert(7, 8);
        queue.insert(8, 1);
        queue.insert(8, 0);
        queue.insert(9, 9);

        TS_ASSERT_EQUALS(queue.getSize(), 10);
    }

    void testPopPeekManual() {
        std::pair < size_t, size_t > elm1 = queue.peekMin();
        queue.popMin();
        std::cout << elm1.first << elm1.second << "\n";
        TS_ASSERT(elm1.first == 6 && elm1.second == 0);

        std::pair < size_t, size_t > elm2 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm2.first == 8 && elm2.second == 0);

        std::pair < size_t, size_t > elm3 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm3.first == 2 && elm3.second == 1);

        std::pair < size_t, size_t > elm4 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm4.first == 10 && elm4.second == 1);

        std::pair < size_t, size_t > elm5 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm5.first == 1 && elm5.second == 3);

        std::pair < size_t, size_t > elm6 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm6.first == 5 && elm6.second == 7);

        std::pair < size_t, size_t > elm7 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm7.first == 7 && elm7.second == 8);

        std::pair < size_t, size_t > elm8 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm8.first == 9 && elm8.second == 9);

        std::pair < size_t, size_t > elm9 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm9.first == 4 && elm9.second == 10);

        std::pair < size_t, size_t > elm10 = queue.peekMin();
        queue.popMin();
        TS_ASSERT(elm10.first == 3 && elm10.second == 15);

        TS_ASSERT_EQUALS(queue.getSize(), 0);
    }
};

#endif                          // PRIORITY_QUEUE_TEST
