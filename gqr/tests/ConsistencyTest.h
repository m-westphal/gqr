// -*- C++ -*-
#ifndef CONSISTENCYTEST_H
#define CONSISTENCYTEST_H

#include <iostream>
#include <string>
#include <map>

#include "Relation.h"

#include "Calculus.h"
#include "CalculusReader.h"

#include "RestartsFramework.h"
#include "gqrtl/CSP.h"
#include "gqrtl/CalculusOperations.h"
#include "gqrtl/WeightedTripleIterator.h"
#include "gqrtl/DFS.h"
#include "gqrtl/RestartingDFS.h"

#include "TestSuite.h"
#include "WorldFixture.h"

typedef gqrtl::CSP<Relation, gqrtl::CalculusOperations<Relation> > CSPSimple;

bool isNormalizedCSP(CSPSimple& csp) {
    for (size_t i = 0; i < csp.getSize(); i++)
	for (size_t j = 0; j < csp.getSize(); j++)
	    if (i == j) {
		if (!(csp.getConstraint(i,j) == csp.getCalculus().getCalculus().getIdentityRelation()
		     || csp.getConstraint(i,j).none())) {
		    return false;
		}
	    }
	    else if (csp.getConstraint(i,j) != csp.getCalculus().getConverse(csp.getConstraint(j, i))) {
		    return false;
	    }
    return true;
}

class ReasoningTest : public CxxTest::TestSuite
{
  Calculus* rcc8;
  Calculus* allen;
  gqrtl::CalculusOperations<Relation>* rcc8_op;
  gqrtl::CalculusOperations<Relation>* allen_op;
  RestartsFramework restarts;

 public:
  void setUp() {
    std::ifstream stream_r;
    const std::string rcc8_s = "./data/rcc8.spec";

    stream_r.open(rcc8_s.c_str());
    TS_ASSERT(stream_r.is_open());

    CalculusReader r1("rcc8", "./data", &stream_r);
    rcc8 = r1.makeCalculus();
    rcc8_op = new gqrtl::CalculusOperations<Relation>(*rcc8);

    std::ifstream stream_a;
    const std::string allen_s = "./data/allen.spec";

    stream_a.open(allen_s.c_str());
    TS_ASSERT(stream_a.is_open());

    CalculusReader r2("allen", "./data", &stream_a);
    allen = r2.makeCalculus();
    allen_op = new gqrtl::CalculusOperations<Relation>(*allen);
  }

  void tearDown() {
    delete rcc8;
    delete rcc8_op;
    delete allen;
    delete allen_op;
  }

  void testAllenInitialConsistent( void ) {
    // initial network should be consistent (since there are only universal relations)
    CSPSimple csp(10, *allen_op, "test");
    TS_ASSERT(isNormalizedCSP(csp));

    gqrtl::DFS<Relation> search(csp, NULL);
    CSPSimple* res = search.run();
    TS_ASSERT(res);
    delete res;

    restarts.initialize();
    gqrtl::RestartingDFS<Relation> search_r(csp, restarts, NULL);
    res = search_r.run();
    TS_ASSERT(res);
    delete res;
  }

  void testRCC8InitialConsistent( void ) {
    // initial network should be consistent
    CSPSimple csp(32, *rcc8_op, "test");
    TS_ASSERT(isNormalizedCSP(csp));

    gqrtl::DFS<Relation> search(csp, NULL);
    CSPSimple* res = search.run();
    TS_ASSERT(res);
    delete res;

    restarts.initialize();
    gqrtl::RestartingDFS<Relation> search_r(csp, restarts, NULL);
    res = search_r.run();
    TS_ASSERT(res);
    delete res;
  }


  void testAllenInconsistent( void ) {
    // Example of an Allen CSP that is path consistent, but not consistent
    CSPSimple csp(4, *allen_op, "test_inconsistent");
    csp.setConstraint(0,1, allen->encodeRelation("d di"));
    csp.setConstraint(0,2, allen->encodeRelation("f fi"));
    csp.setConstraint(0,3, allen->encodeRelation("si mi"));
    csp.setConstraint(1,2, allen->encodeRelation("d di"));
    csp.setConstraint(1,3, allen->encodeRelation("oi"));
    csp.setConstraint(2,3, allen->encodeRelation("si mi"));

    TS_ASSERT(isNormalizedCSP(csp));

    gqrtl::DFS<Relation> search(csp, NULL);
    TS_ASSERT(!search.run());

    restarts.initialize();
    gqrtl::RestartingDFS<Relation> search_r(csp, restarts, NULL);
    TS_ASSERT(!search_r.run());
  }

  void testAllenConsistent( void ) {
    // simple consistent network (set "s" on all constraints)
    CSPSimple csp(10, *allen_op, "test_consistent");
    csp.setConstraint(0,1, allen->encodeRelation("s"));
    csp.setConstraint(1,2, allen->encodeRelation("s"));
    csp.setConstraint(2,3, allen->encodeRelation("s d mi"));
    csp.setConstraint(3,4, allen->encodeRelation("s o m mi fi f d di >"));
    csp.setConstraint(4,5, allen->encodeRelation("s"));
    csp.setConstraint(6,7, allen->encodeRelation("s oi o m >"));
    csp.setConstraint(7,8, allen->encodeRelation("s d m o f"));
    csp.setConstraint(7,9, allen->encodeRelation("s fi f o oi >"));

    csp.setConstraint(0,9, allen->encodeRelation("s f di m"));
    csp.setConstraint(0,2, allen->encodeRelation("s < >"));

    TS_ASSERT(isNormalizedCSP(csp));

    gqrtl::DFS<Relation> search(csp, NULL);
    CSPSimple* res = search.run();
    TS_ASSERT(res);
    delete res;

    restarts.initialize();
    gqrtl::RestartingDFS<Relation> search_r(csp, restarts, NULL);
    res = search_r.run();
    TS_ASSERT(res);
    delete res;
  }

  void testRCC8Consistent( void ) {
    // simple consistent network (set "TPP" on all constraints)
    CSPSimple csp(50, *rcc8_op, "test_consistent");

    // NOTE: Compiling CSP in debug mode, i.e., linear memory requirements in search depth!
    // ... so keep CSP size low
    for (size_t i = 0; i < 49; i++) {
	for (size_t j = 0; j < 50; j++) {
	    if (i == j)
		continue;

	    Relation relation = rcc8->encodeRelation("TPP");
	    // make relation look more complicated
	    for (size_t k = 0; k < (i+j)%8; k++) {
		Relation s;
		s.set((i+j+k)%7+1);
		relation |= s;
	    }

	    csp.setConstraint(i, j, relation);
	}
    }

    TS_ASSERT(isNormalizedCSP(csp));

    gqrtl::DFS<Relation> search(csp, NULL);
    CSPSimple* res = search.run();
    TS_ASSERT(res);
    delete res;

    restarts.initialize();
    gqrtl::RestartingDFS<Relation> search_r(csp, restarts, NULL);
    res = search_r.run();
    TS_ASSERT(res);
    delete res;
  }

  void testRCC8NotPathConsistent( void ) {
    // simple inconsistent network
    CSPSimple csp(3, *rcc8_op, "test_not_pc");
    csp.setConstraint(0,1, rcc8->encodeRelation("TPP DC"));
    csp.setConstraint(1,2, rcc8->encodeRelation("TPP EC"));
    csp.setConstraint(0,2, rcc8->encodeRelation("TPPI EQ"));

    TS_ASSERT(isNormalizedCSP(csp));
    gqrtl::DFS<Relation> search(csp, NULL);
    TS_ASSERT(!search.run());

    restarts.initialize();
    gqrtl::RestartingDFS<Relation> search_r(csp, restarts, NULL);
    TS_ASSERT(!search_r.run());
  }

};
#endif // CONSISTENCYTEST_H
