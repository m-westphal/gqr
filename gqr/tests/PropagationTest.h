// -*- C++ -*-
#ifndef PROPAGATION_TEST_H
#define PROPAGATION_TEST_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "Relation.h"
#include "Calculus.h"
#include "CalculusReader.h"
#include "TestSuite.h"
#include "WorldFixture.h"

#include "gqrtl/CSP.h"
#include "gqrtl/WeightedTripleIterator.h"

bool isNormalizedCSP(gqrtl::CSP<Relation, Calculus>& csp) {
    for (size_t i = 0; i < csp.getSize(); i++)
	for (size_t j = 0; j < csp.getSize(); j++)
	    if (i == j) {
		if (!(csp.getConstraint(i,j) == csp.getCalculus().getIdentityRelation()
		     || csp.getConstraint(i,j) == Relation())) {
		    return false;
		}
	    }
	    else if (csp.getConstraint(i,j) != csp.getCalculus().getConverse(csp.getConstraint(j, i))) {
		    return false;
	    }
    return true;
}

bool isUnconstrainedCSP(gqrtl::CSP<Relation,Calculus>& csp) {
    for (size_t i = 0; i < csp.getSize(); i++)
	for (size_t j = 0; j < csp.getSize(); j++) {
	    if (i == j && csp.getConstraint(i,j) != csp.getCalculus().getIdentityRelation())
		return false;
	    if (i != j && csp.getCalculus().getUniversalRelation() != csp.getConstraint(i,j))
		return false;
	}
    return true;
}

class ReasoningTest : public CxxTest::TestSuite
{
  Calculus* rcc8;
  Calculus* allen;

  gqrtl::WeightedTripleIterator<Relation, gqrtl::CSP<Relation, Calculus> > ac;

 public:
  void setUp() {
    Relation::init();

    const std::string data_dir = "./data";

    std::ifstream input;
    const std::string filename = data_dir + "/allen.spec";

    input.open(filename.c_str());
    TS_ASSERT(input.is_open());

    CalculusReader reader("allen", data_dir, &input);
    allen = reader.makeCalculus();
  }

  void tearDown()
  {
    Relation::clean_up();

    delete rcc8;
    delete allen;
  }

  void testAllenInitialConsistent( void ) {
    // initial network should be consistent (since there are only universal relations)
    gqrtl::CSP<Relation,Calculus> csp(32, *allen, "initial");
    TS_ASSERT(isNormalizedCSP(csp));
    TS_ASSERT(isUnconstrainedCSP(csp));

    TS_ASSERT(ac.enforce(csp).empty());
    TS_ASSERT(isUnconstrainedCSP(csp));

    TS_ASSERT(ac.enforce(csp,10,5).empty());
    TS_ASSERT(isUnconstrainedCSP(csp));
  }


  void testAllenInconsistent( void ) {
    // Example of an Allen CSP that is path consistent, but not consistent
    gqrtl::CSP<Relation,Calculus> csp(4, *allen, "incons");
    csp.setConstraint(0,1, allen->encodeRelation("d di"));
    csp.setConstraint(0,2, allen->encodeRelation("f fi"));
    csp.setConstraint(0,3, allen->encodeRelation("si mi"));
    csp.setConstraint(1,2, allen->encodeRelation("d di"));
    csp.setConstraint(1,3, allen->encodeRelation("oi"));
    csp.setConstraint(2,3, allen->encodeRelation("si mi"));

    gqrtl::CSP<Relation, Calculus> clone_a = csp;
    gqrtl::CSP<Relation, Calculus> clone_b = csp;
    gqrtl::CSP<Relation, Calculus> clone_c = csp;
    gqrtl::CSP<Relation, Calculus> clone_d = csp;

    TS_ASSERT(isNormalizedCSP(csp));

    TS_ASSERT(ac.enforce(clone_a).empty());
    TS_ASSERT(isNormalizedCSP(clone_a));

    TS_ASSERT(ac.enforce(clone_b).empty());
    TS_ASSERT(isNormalizedCSP(clone_b));

    TS_ASSERT(clone_a == clone_b);

    TS_ASSERT(clone_c == clone_d);
  }

  void testAllenConsistent( void ) {
    // simple consistent network (set "s" on all constraints)
    gqrtl::CSP<Relation, Calculus> csp(10, *allen, "cons");
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
    TS_ASSERT(ac.enforce(csp).empty());
    TS_ASSERT(isNormalizedCSP(csp));
  }


};
#endif // PROPAGATION_TEST_H
