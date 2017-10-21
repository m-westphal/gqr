// -*- C++ -*-

// test libgqr interface (currently only the C interface)
/** @todo: base on C interface OR additionally test C interface as well */
#ifndef LIBGQR_TEST_H
#define LIBGQR_TEST_H

#include <string>
#include <utility>

// GQR core
#include "gqr.h"
#include "RelationFixedBitset.h"
#include "Tuple.h"
#include "Calculus.h"
#include "CalculusReader.h"
#include "BaseRelationSplitter.h"
#include "FileSplitter.h"
#include "CSP.h"
#include "CSPReader.h"
#include "WeightedAlgebraicClosure.h"
#include "WeightValueOrder.h"
#include "WeightOverLearnedWeightsEdgeQueue.h"
#include "Consistency-2-way.h"

// C++ abstracted interface
#include "gqr_wrap.h"

// consistency depends on timer
/** @todo: remove timer from consistency if possible */
#include "Timer.h"
//#include "Timer.cc"

#include "WorldFixture.h"
#include "TestSuite.h"

class libgqrTest : public CxxTest::TestSuite {
    public:
        void setUp() { }

        void tearDown() { }

        void testCalculusRCC8() {
            GQR_Calculus test_calc;
            TS_ASSERT(!test_calc.set_calculus("rcc0", "data")); // should not exist
            TS_ASSERT(test_calc.set_calculus("rcc8", "data")); // should exist
            TS_ASSERT(!test_calc.set_calculus("rcc8", "data")); // refuse reinit

            const std::string rcc8_base_relations = test_calc.get_base_relations();
            TS_ASSERT(rcc8_base_relations.find(" EQ ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find(" DC ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find(" PO ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find(" TPP ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find(" TPPI ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find(" NTPP ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find(" NTPPI ") != std::string::npos);

            TS_ASSERT_EQUALS(test_calc.get_composition("( EQ )", "( PO )"), "( PO )");
            TS_ASSERT_EQUALS(test_calc.get_composition("( DC )", "( EQ )"), "( DC )");
            TS_ASSERT_EQUALS(test_calc.get_composition("( DC )", "( TPPI  )"), "( DC )");

            TS_ASSERT_EQUALS(test_calc.get_converse("( TPP )"), "( TPPI )");
            TS_ASSERT_EQUALS(test_calc.get_converse("( NTPPI )"), "( NTPP )");
        }
        void testCalculusOPRA2() {
            GQR_Calculus test_calc;
            TS_ASSERT(!test_calc.set_calculus("opra0", "data")); // should not exist
            TS_ASSERT(test_calc.set_calculus("opra2", "data")); // should exist
            TS_ASSERT(!test_calc.set_calculus("rcc8", "data")); // refuse reinit

            const std::string opra2_base_relations = test_calc.get_base_relations();

            size_t pos = 0;
            size_t count = 0;
            while ( (pos=opra2_base_relations.find(' ', pos)) != std::string::npos) {
                pos++;
                count++;
            }
            TS_ASSERT_EQUALS(count, 72+1); // should contain 72 names

            TS_ASSERT_EQUALS(test_calc.get_composition("( EQ )", "( PO )"), "( )"); //  unknown base relations
            TS_ASSERT_EQUALS(test_calc.get_composition("( 7_7 )", "( s_6)"), "( 7_1 )");
            TS_ASSERT_EQUALS(test_calc.get_composition("( 7_4 )", "( s_0 )"), "( 7_4 )");

            TS_ASSERT_EQUALS(test_calc.get_converse("( 1_6 )"), "( 6_1 )");
            TS_ASSERT_EQUALS(test_calc.get_converse("( s_2 )"), "( s_6 )");
        }

        void testCSPRCC8() {
            GQR_Calculus rcc8;
            TS_ASSERT(rcc8.set_calculus("rcc8", "data"));

            GQR_CSP csp(3, rcc8);
            csp.setName("test");
            TS_ASSERT_EQUALS(csp.getName(), "test");
            TS_ASSERT_EQUALS(csp.getSize(), 3);

            GQR_CSP copy(csp);
            TS_ASSERT_EQUALS(copy.getName(), "test");
            TS_ASSERT_EQUALS(copy.getSize(), 3);
            TS_ASSERT(csp.check_equals(copy));
            TS_ASSERT(!csp.check_refines(copy));

            csp.add_constraint(0, 1, "( TPP )");
            csp.add_constraint(1, 2, "( TPP )");
            csp.add_constraint(0, 2, "( TPP )");
            csp.add_constraint(3, 5, "( EQ )"); // should be silently ignored

            TS_ASSERT_EQUALS(csp.get_constraint(0, 1), "( TPP )");
            TS_ASSERT_EQUALS(csp.get_constraint(3, 3), "");
            TS_ASSERT(csp.check_refines(copy));

            copy.add_constraint(0, 1, "( PO )");
            TS_ASSERT(!csp.check_refines(copy));
            copy.add_constraint(0, 1, "( EQ )");
            TS_ASSERT_EQUALS(copy.get_constraint(0, 1), "( )");
            copy.set_constraint(0, 1, "( TPP )");
            TS_ASSERT_EQUALS(copy.get_constraint(0, 1), "( TPP )");
        }

        void testSolverIA() {
            GQR_Calculus allen;
            TS_ASSERT(allen.set_calculus("allen", "data"));

            GQR_CSP unsat(3, allen);
            unsat.set_constraint(0, 1, "( < )");
            unsat.set_constraint(1, 2, "( < )");
            unsat.set_constraint(0, 2, "( = )");

            GQR_CSP cpy(unsat);

            GQR_Solver solver(allen);

            TS_ASSERT_EQUALS(solver.enforce_algebraic_closure(unsat), false);
            TS_ASSERT(unsat.check_refines(cpy));

            TS_ASSERT(solver.get_scenario(unsat) == NULL);
            TS_ASSERT(solver.get_scenario(cpy) == NULL);

            GQR_CSP sat(5, allen);
            sat.set_constraint(0,1, "( < )");
            sat.set_constraint(2,3, "( < )");
            sat.set_constraint(3,4, "( < )");

            GQR_CSP sat_cpy(sat);
            TS_ASSERT_EQUALS(solver.enforce_algebraic_closure(sat), true);
            TS_ASSERT_EQUALS(sat.check_equals(sat_cpy), false);
            sat_cpy.add_constraint(2,4, "( < )");
            TS_ASSERT_EQUALS(sat.check_equals(sat_cpy), true);

            GQR_CSP* solution1 = solver.get_scenario(sat);
            TS_ASSERT( solution1 != NULL );

            TS_ASSERT_EQUALS(solver.set_tractable_subclass("horn"), false);
            TS_ASSERT_EQUALS(solver.set_tractable_subclass("data/allen/calculus/hornalg"), true);
            GQR_CSP* solution2 = solver.get_scenario(sat_cpy);
            TS_ASSERT( solution2 != NULL );
            TS_ASSERT_EQUALS( solution1->check_equals(*solution2), false );
	    TS_ASSERT_EQUALS( solution2->get_constraint(1,2), allen.get_base_relations() ); // universal relation is in ORD-horn
            free(solution1);
            free(solution2);
        }
};

#endif // LIBGQR_TEST_H
