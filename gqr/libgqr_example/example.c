/* A simple libgqr C example */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "libgqr.h"

static void print_csp(GqrCsp* csp) {
	char * name = gqr_csp_get_name(csp);
	printf("%s\n", name);
	free(name);

	int i, j;
	for (i = 0; i < gqr_csp_get_size(csp); i++) {
		for (j = i; j < gqr_csp_get_size(csp); j++) {
			char* str = gqr_csp_get_constraint(csp, i, j);
			printf("\t%d, %d\t%s\n", i, j, str);
			free(str);
		}
	}
}

int main(void) {
	g_type_init(); /* gobject init (required!) */

	/* load Allen's Interval calculus */
	GqrCalculus* allen = gqr_calculus_new("allen", "../../data");

	if (allen == NULL) {
		printf("Could not load allen calculus. Tried \"data/allen.spec\"\n");
		return -1;
	}

	/* generate new csp */
	GqrCsp* csp = gqr_csp_new(4, allen);
	assert(csp != NULL);

	gqr_csp_set_name(csp, "Example CSP");

	/* add some constraints */
	gqr_csp_add_constraint(csp, 0, 1, "( < )");
	gqr_csp_add_constraint(csp, 2, 4, "( m )");
	gqr_csp_add_constraint(csp, 2, 3, "( di )");

	print_csp(csp);

	/* establish path consistency */
	GqrSolver* solver = gqr_solver_new(allen);

	bool ret = gqr_solver_enforce_algebraic_closure(solver, csp);
	assert(ret); /* the example is path consistent */

	/* get and output a scenario of 'csp' */
	GqrCsp* res = gqr_solver_get_scenario(solver, csp);
	assert(res); /* the example is satisfiable */

	printf("One scenario is:\n");
	print_csp(res);

	/* clean up */
	gqr_solver_unref(solver);
	gqr_csp_unref(res);
	gqr_csp_unref(csp);
	gqr_calculus_unref(allen);

	return 0;
}
