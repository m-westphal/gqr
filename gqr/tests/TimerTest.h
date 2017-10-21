// -*- C++ -*-
#ifndef TIMETEST_H
#define TIMETEST_H

#include "TestSuite.h"

#include "utils/Timer.h"

#include <math.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <stdlib.h>


static bool test_msec_passed(const Timer& a, const Timer& b) { // requires two time points where a < b.
    TS_ASSERT(a.msec_passed(b) < 0);
    TS_ASSERT(b.msec_passed(a) > 0);
    TS_ASSERT(labs(a.msec_passed(b) + b.msec_passed(a)) <= 2); // allow for some rounding errors

    const long int passed = b.msec_passed(a); // time passed between a and b

    // recalculate time point b based on a and passed time (might include rounding error)
    const long int b_msec_r = (a.getTime().tv_sec * 1000) + (a.getTime().tv_usec / 1000) + passed;
    const long int b_msec   = (b.getTime().tv_sec * 1000) + (b.getTime().tv_usec / 1000);
    TS_ASSERT( b_msec - b_msec_r <= 2);
}

static void test_once(const long int i) { // generate and test a pair of two distinct time points at least i msec apart
    const Timer start = Timer();

    rusage init;
    TS_ASSERT(!getrusage(RUSAGE_SELF, &init));

    const long int start_msec = (init.ru_utime.tv_sec+init.ru_stime.tv_sec) * 1000 + (init.ru_utime.tv_usec+init.ru_stime.tv_usec) / 1000 + 1;

    // keep CPU busy for some time
    while (true) {
	rusage stats;
	TS_ASSERT(!getrusage(RUSAGE_SELF, &stats));

	const long int passed_msec = (stats.ru_utime.tv_sec+stats.ru_stime.tv_sec) * 1000
					+ (stats.ru_utime.tv_usec+stats.ru_stime.tv_usec) / 1000
					- start_msec;

	if (passed_msec >= i)
	    break;
    }

    const Timer finish = Timer();
    test_msec_passed(start, finish);
}

class TimerTest : public CxxTest::TestSuite {

 public:
  void setUp() { }

  void tearDown() { }

  void testPassed( void ) {
    Timer start = Timer();
    for (size_t i = 1; i <= 10; i++)
	test_once(3*i);
    test_msec_passed(start, Timer());
  }

};

#endif // TIMERTEST_H
