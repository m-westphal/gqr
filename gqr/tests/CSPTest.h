// -*- C++ -*-
#ifndef CSP_TEST_H
#define CSP_TEST_H

#include <cassert>
#include <iostream>

#include "Relation.h"
#include "gqrtl/CSP.h"
#include "Tuple.h"

#include "TestSuite.h"

class FakeCalculus {
	public:
		static Relation getUniversalRelation() {
			Relation u;
			u.set(3);
			u.set(2);
			u.set(1);
			return u;
		}
		static Relation getIdentityRelation() {
			Relation i;
			i.set(1);
			return i;
		}
		static Relation getConverse(const Relation& c) {
			return Relation(c);
		}
};

class CSPTest : public CxxTest::TestSuite {
	public:
		void setUp() {}

		void tearDown() {}

		void testMatrixInit( void ) {
			FakeCalculus f;
			gqrtl::CSP<Relation, FakeCalculus> test(10, f, "test1");
			TS_ASSERT_EQUALS(10, test.getSize());
			for (size_t i = 0; i < 10; ++i)
				for (size_t j = 0; j < 10; ++j) {
					if (i == j) {
						TS_ASSERT_EQUALS(test.getConstraint(i,j), f.getIdentityRelation());
					} else {
						TS_ASSERT_EQUALS(test.getConstraint(i,j), f.getUniversalRelation());
					}
				}
		}

		void testSize( void ) {
			FakeCalculus f;
			gqrtl::CSP<Relation, FakeCalculus> test1(11, f, "test1");
			TS_ASSERT_EQUALS(11, test1.getSize());
			gqrtl::CSP<Relation, FakeCalculus> test2(3, f, "test2");
			TS_ASSERT_EQUALS(3, test2.getSize());
		}

		void testName( void ) {
			FakeCalculus f;
			gqrtl::CSP<Relation, FakeCalculus> test1(3, f, "test1");
			TS_ASSERT_EQUALS(test1.name, "test1");
		}

		void testGetCalculus( void ) {
			FakeCalculus f;
			gqrtl::CSP<Relation, FakeCalculus> test1(3, f, "test1");
			TS_ASSERT_EQUALS(&test1.getCalculus(), &f);
		}


		void testSetGetConstraint( void ) {
			FakeCalculus f;
			gqrtl::CSP<Relation, FakeCalculus> test1(10, f, "test1");

			Relation a;
			a.set(2);

			test1.setConstraint(0, 1, a);

			for (size_t i = 0; i < 10; ++i)
				for (size_t j = 0; j < 10; ++j) {
					if (i+j == 1) {
						TS_ASSERT_EQUALS(test1.getConstraint(i,j), a);
					}
					else {
						if (i == j) {
							TS_ASSERT_EQUALS(test1.getConstraint(i,j), f.getIdentityRelation());
						} else {
							TS_ASSERT_EQUALS(test1.getConstraint(i,j), f.getUniversalRelation());
						}
					}
				}
		}

		void testCopyConstructor( void ) {
			// test whether we really have a deep copy
			FakeCalculus f;
			gqrtl::CSP<Relation, FakeCalculus> csp(10, f, "test1");

			gqrtl::CSP<Relation, FakeCalculus> clone(csp);

			// matrix: deep copy

			for (size_t i = 0; i < 10; ++i)
				for (size_t j = 0; j < 10; ++j)
					TS_ASSERT_EQUALS(csp.getConstraint(i,j), clone.getConstraint(i,j));


			clone.setConstraint(0,0, Relation());

			TS_ASSERT_DIFFERS(csp.getConstraint(0,0), clone.getConstraint(0,0));

			for (size_t i = 0; i < 10; ++i)
				for (size_t j = 1; j < 10; ++j)
					TS_ASSERT_EQUALS(csp.getConstraint(i,j), clone.getConstraint(i,j));
		}

};
#endif // CSP_TEST_H
