// -*- C++ -*-
#ifndef STRINGTOOLSTEST_H
#define STRINGTOOLSTEST_H

#include "TestSuite.h"

#include "Stringtools.h"

class StringtoolsTest : public CxxTest::TestSuite {

  const std::vector<std::string>* v;

 public:
  void setUp() {
  }

  void tearDown() {
    delete v;
  }

  void testStringtools::splitEmpty( void ) {
    v = Stringtools::split("", ' ');
    TS_ASSERT_EQUALS(0U, v->size());
  }

  void testStringtools::splitNoSplit( void ) {
    v = Stringtools::split("lalala", ' ');
    TS_ASSERT_EQUALS(1U, v->size());
    TS_ASSERT_EQUALS("lalala", (*v)[0]);
  }

  void testStringtools::splitThree( void ) {
    v = Stringtools::split("la la la", ' ');
    TS_ASSERT_EQUALS(3U, v->size());
    TS_ASSERT_EQUALS("la", (*v)[0]);
    TS_ASSERT_EQUALS("la", (*v)[1]);
    TS_ASSERT_EQUALS("la", (*v)[2]);
  }

  void testStringtools::splitBegin( void ) {
    v = Stringtools::split(" la la la", ' ');
    TS_ASSERT_EQUALS(3U, v->size());
    TS_ASSERT_EQUALS("la", (*v)[0]);
    TS_ASSERT_EQUALS("la", (*v)[1]);
    TS_ASSERT_EQUALS("la", (*v)[2]);
  }

  void testStringtools::splitEnd( void ) {
    v = Stringtools::split("la la la ", ' ');
    TS_ASSERT_EQUALS(3U, v->size());
    TS_ASSERT_EQUALS("la", (*v)[0]);
    TS_ASSERT_EQUALS("la", (*v)[1]);
    TS_ASSERT_EQUALS("la", (*v)[2]);
  }

  void testStringtools::splitBoth( void ) {
    v = Stringtools::split(" la la la ", ' ');
    TS_ASSERT_EQUALS(3U, v->size());
    TS_ASSERT_EQUALS("la", (*v)[0]);
    TS_ASSERT_EQUALS("la", (*v)[1]);
    TS_ASSERT_EQUALS("la", (*v)[2]);
  }

};

#endif // STRINGTOOLSTEST_H
