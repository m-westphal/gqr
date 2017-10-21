// test gobject interface
/** @todo: base on C interface OR additionally test C interface as well */
#ifndef GOBJECTTEST_H
#define GOBJECTTEST_H

// gobject abstracted interface
#include "gobject/gqrcalculus.h"
#include "gobject/gqrcsp.h"
#include "gobject/gqrsolver.h"

// consistency depends on timer
/** @todo: remove timer from consistency if possible */
#include "Timer.h"
//#include "Timer.cc"

#include <string>

#include "WorldFixture.h"
#include "TestSuite.h"

class gobjectTest : public CxxTest::TestSuite {
    public:
        void setUp() {
            g_type_init();
        }

        void tearDown() { }

        void testCalculusRCC8() {
            GqrCalculus *test_calc = gqr_calculus_new((char *) "rcc8", (char *) "data");
            TS_ASSERT(test_calc); // should exist
            TS_ASSERT(gqr_calculus_new((char *) "rcc0", (char *) "data") == NULL); // should not exist

            const std::string rcc8_base_relations = std::string(gqr_calculus_get_base_relations(test_calc));
            TS_ASSERT(rcc8_base_relations.find((char *) " EQ ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find((char *) " DC ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find((char *) " PO ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find((char *) " TPP ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find((char *) " TPPI ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find((char *) " NTPP ") != std::string::npos);
            TS_ASSERT(rcc8_base_relations.find((char *) " NTPPI ") != std::string::npos);

            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_composition(test_calc, (char *) "( EQ )", (char *) "( PO )")), "( PO )");
            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_composition(test_calc, (char *) "( DC )", (char *) "( EQ )")), "( DC )");
            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_composition(test_calc, (char *) "( DC )", (char *) "( TPPI  )")), "( DC )");

            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_converse(test_calc, (char *) "( TPP )")), "( TPPI )");
            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_converse(test_calc, (char *) "( NTPPI )")), "( NTPP )");

            gqr_calculus_unref(test_calc);
        }
        void testCalculusOPRA2() {
            GqrCalculus *test_calc = gqr_calculus_new((char *) "opra2", (char *) "data");
            TS_ASSERT(test_calc);
            TS_ASSERT(gqr_calculus_new((char *) "opra0", (char *) "data") == NULL); // should not exist

            const std::string opra2_base_relations = std::string(gqr_calculus_get_base_relations(test_calc));

            size_t pos = 0;
            size_t count = 0;
            while ( (pos=opra2_base_relations.find(' ', pos)) != std::string::npos) {
                pos++;
                count++;
            }
            TS_ASSERT_EQUALS(count, 72+1); // should contain 72 names

            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_composition(test_calc, (char *) "( EQ )", (char *) "( PO )")), "( )"); //  unknown base relations
            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_composition(test_calc, (char *) "( 7_7 )", (char *) "( s_6)")), "( 7_1 )");
            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_composition(test_calc, (char *) "( 7_4 )", (char *) "( s_0 )")), "( 7_4 )");

            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_converse(test_calc, (char *) "( 1_6 )")), "( 6_1 )");
            TS_ASSERT_EQUALS(std::string(gqr_calculus_get_converse(test_calc, (char *) "( s_2 )")), "( s_6 )");

            gqr_calculus_unref(test_calc);
        }

        void testCSPRCC8() {
            GqrCalculus *rcc8 = gqr_calculus_new((char *) "rcc8", (char *) "data");
            TS_ASSERT(rcc8);

            GqrCsp *csp = gqr_csp_new(3, rcc8);
            gqr_csp_set_name(csp, (char *) "test");
            TS_ASSERT_EQUALS(std::string(gqr_csp_get_name(csp)), "test");
            TS_ASSERT_EQUALS(gqr_csp_get_size(csp), 3);

//            GQR_CSP copy(csp);
//            TS_ASSERT_EQUALS(copy.getName(), "test");
//            TS_ASSERT_EQUALS(copy.getSize(), 3);
//            TS_ASSERT(csp.check_equals(copy));
//            TS_ASSERT(!csp.check_refines(copy));

            gqr_csp_add_constraint(csp, 0, 1, (char *) "( TPP )");
            gqr_csp_add_constraint(csp, 1, 2, (char *) "( TPP )");
            gqr_csp_add_constraint(csp, 0, 2, (char *) "( TPP )");
            gqr_csp_add_constraint(csp, 3, 5, (char *) "( EQ )");

            TS_ASSERT_EQUALS(std::string(gqr_csp_get_constraint(csp, 0, 1)), "( TPP )");
            TS_ASSERT_EQUALS(std::string(gqr_csp_get_constraint(csp, 3, 3)), "");
//            TS_ASSERT(csp.check_refines(copy));
//
//            copy.add_constraint(0, 1, "( PO )");
//            TS_ASSERT(!csp.check_refines(copy));
//            copy.add_constraint(0, 1, "( EQ )");
//            TS_ASSERT_EQUALS(copy.get_constraint(0, 1), "( )");
//            copy.set_constraint(0, 1, "( TPP )");
//            TS_ASSERT_EQUALS(copy.get_constraint(0, 1), "( TPP )");
            gqr_calculus_unref(rcc8);
        }

        void testSolverIA() {
            GqrCalculus *allen = gqr_calculus_new((char *) "allen", (char *) "data");
            TS_ASSERT(allen);

            GqrCsp *unsat(gqr_csp_new(3, allen));
            gqr_csp_set_constraint(unsat, 0, 1, (char *) "( < )");
            gqr_csp_set_constraint(unsat, 1, 2, (char *) "( < )");
            gqr_csp_set_constraint(unsat, 0, 2, (char *) "( = )");

            GqrSolver *solver = gqr_solver_new(allen);

            TS_ASSERT_EQUALS(gqr_solver_enforce_algebraic_closure(solver, unsat), false);

            TS_ASSERT(gqr_solver_get_scenario(solver, unsat) == NULL);

            GqrCsp *sat = gqr_csp_new(5, allen);
            gqr_csp_set_constraint(sat, 0, 1, (char *) "( < )");
            gqr_csp_set_constraint(sat, 2, 3, (char *) "( < )");
            gqr_csp_set_constraint(sat, 3, 4, (char *) "( < )");

            GqrCsp *solution1 = gqr_solver_get_scenario(solver, sat);
            TS_ASSERT( solution1 != NULL );

            TS_ASSERT_EQUALS(gqr_solver_set_tractable_subclass(solver, (char *) "horn"), false);
            TS_ASSERT_EQUALS(gqr_solver_set_tractable_subclass(solver, (char *) "data/allen/calculus/hornalg"), true);
            gqr_csp_unref(solution1);
            gqr_csp_unref(sat);
            gqr_solver_unref(solver);
            gqr_csp_unref(unsat);
            gqr_calculus_unref(allen);
        }
};


#endif /* GOBJECTTEST_H_ */
