// -*- C++ -*-
#ifndef RELATION_TEST_H
#define RELATION_TEST_H

#include "Relation.h"

#include "TestSuite.h"

class RelationTest : public CxxTest::TestSuite
{
	private:
		Relation empty;

	public:
		void setUp() {
			empty = Relation();
		}

		void tearDown() {}

		void testZeroInit( void ) {
			TS_ASSERT(empty.none());
			TS_ASSERT_EQUALS(empty.count(), 0);
			TS_ASSERT_EQUALS(empty[0], false);
			TS_ASSERT_EQUALS(empty[1], false);
			TS_ASSERT_EQUALS(empty[2], false);
			TS_ASSERT_EQUALS(empty[3], false);
			TS_ASSERT_EQUALS(empty, Relation());

			Relation t;
			t.set(0);
			TS_ASSERT_DIFFERS(empty, t);
		}

		void testEqualityInequality( void ) {
			Relation a;
			a.set(0);
			a.set(1);
			a.set(3);

			Relation b;
			b.set(0);
			b.set(1);
			b.set(3);

			Relation c;
			c.set(0);
			c.set(2);
			c.set(3);

			TS_ASSERT_DIFFERS(empty, a);
			TS_ASSERT_DIFFERS(empty, b);
			TS_ASSERT_DIFFERS(empty, c);

			TS_ASSERT_DIFFERS(a, c);
			TS_ASSERT_DIFFERS(b, c);

			TS_ASSERT_EQUALS(a, b);

			a.set(2);
			c.set(1);
			TS_ASSERT_EQUALS(a, c);
		}

		void testNone( void ) {
			TS_ASSERT(empty.none());
			TS_ASSERT(Relation().none());
		}

		void testArrayAndSetOperators( void ) {
			Relation test;

			for(size_t i = 1000; i > 13; --i) {
				test.set(i);
				TS_ASSERT(test[i]);
			}
			for(size_t i = 1000; i > 13; --i) {
				TS_ASSERT(test[i]);
			}

			TS_ASSERT_EQUALS(test.none(), false);

			for(size_t i = 0; i <= 13; ++i) {
				TS_ASSERT_EQUALS(test[i], false);
			}

			for(size_t i = 500; i > 13; --i) {
				TS_ASSERT(test[i]);
				test.unset(i);
				TS_ASSERT_EQUALS(test[i], false);
			}
			for(size_t i = 500; i > 13; --i) {	// DOUBLE REMOVE
				TS_ASSERT_EQUALS(test[i], false);
				test.unset(i);
				TS_ASSERT_EQUALS(test[i], false);
			}
			for(size_t i = 1000; i > 500; --i) {
				TS_ASSERT(test[i]);
			}
			for(size_t i = 501; i <= 1000; --i) {
				test.set(i);
				TS_ASSERT(test[i]);
			}
		}

		void testCardinality( void ) {
			Relation test;

			TS_ASSERT_EQUALS(test.count(), 0);

			for (size_t i = 0; i < 10; i++) {
				test.set(i);
				TS_ASSERT_EQUALS(test.count(), i+1);
			}
			TS_ASSERT(!test.none());
			for (size_t i = 0; i < 10; i++) {
				test.set(i);
				TS_ASSERT_EQUALS(test.count(), 10);
			}
			TS_ASSERT(!test.none());
			for (size_t i = 0; i < 10; i++) {
				test.unset(i);
				TS_ASSERT_EQUALS(test.count(), 10-(i+1));
			}
			TS_ASSERT_EQUALS(test.count(), 0);
			TS_ASSERT(test.none());
		}

		void testOrOperator( void ) {
			Relation a;
			a.set(0);
			a.set(2);
			a.set(5);

			Relation b;
			b.set(10);
			b.set(13);

			Relation res = a | b;

			TS_ASSERT(res[0]);
			TS_ASSERT(res[2]);
			TS_ASSERT(res[5]);
			TS_ASSERT(res[10]);
			TS_ASSERT(res[13]);

			TS_ASSERT_EQUALS(res.count(), 5);
			res |= empty;
			TS_ASSERT_EQUALS(res.count(), 5);
		}


		void testAndOperator( void ) {
			Relation a;
			a.set(0);
			a.set(2);
			a.set(5);

			Relation b;
			b.set(0);
			b.set(10);
			b.set(13);

			Relation res = a & b;

			TS_ASSERT_EQUALS(res.count(), 1);
			TS_ASSERT(res[0]);
			TS_ASSERT(!res[2]);
			TS_ASSERT(!res[5]);
			TS_ASSERT(!res[10]);
			TS_ASSERT(!res[13]);

			res = res & empty;
			TS_ASSERT(res.none());
		}

		void testSubset( void ) {
			Relation a;
			Relation b;

			a.set(0);
			a.set(1);
			a.set(2);

			b.set(0);
			b.set(1);

			TS_ASSERT(!a.isSubsetOf(b));
			TS_ASSERT(b.isSubsetOf(a));
			a.unset(2);
			TS_ASSERT(a.isSubsetOf(b));
			TS_ASSERT(b.isSubsetOf(a));
		}
};
#endif // RELATION_TEST_H
