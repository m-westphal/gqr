// -*- C++ -*-
#ifndef TUPLE_TEST_H
#define TUPLE_TEST_H

#include "Tuple.h"

#include "TestSuite.h"


class TupleTest : public CxxTest::TestSuite
{

 public:
  void setUp() {
  }

  void tearDown() {
  }

  void testEquals( void ) {
    Tuple t1 = Tuple(2, 4);
    Tuple t2 = Tuple(2, 4);
    TS_ASSERT_EQUALS(t1, t2);
  }

  void testHash( void ) {
    Tuple t1 = Tuple(100,200);
    Tuple t2 = Tuple(100,200);
    TS_ASSERT_EQUALS(Tuple::hash(t1), Tuple::hash(t1));
    TS_ASSERT_EQUALS(Tuple::hash(t1), Tuple::hash(t2));
    TS_ASSERT_EQUALS(Tuple::hash(t2), Tuple::hash(t2));
  }

};

#endif // TUPLE_TEST_H
