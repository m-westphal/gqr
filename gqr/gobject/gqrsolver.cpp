/*
 * gqrsolver.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: thomas
 */

#include <cstring>
#include <stdlib.h>

#include "gqrsolver.h"
#include "gqrcsp.h"

extern "C" {
G_DEFINE_TYPE(GqrSolver, gqr_solver, G_TYPE_OBJECT)
}

/*
 * Private member stuff
 */
#define GQR_SOLVER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GQR_TYPE_SOLVER, GqrSolverPrivate))

struct _GqrSolverPrivate
{
    GQR_Solver *solver;
};


/*
 * Constructor
 */
static void gqr_solver_init(GqrSolver *self)
{
    self->priv = GQR_SOLVER_GET_PRIVATE(self);
}

/*
 * Destructor
 */
static void gqr_solver_finalize(GObject *gobject)
{
    GqrSolver *self = GQR_SOLVER(gobject);

    delete self->priv->solver;

    /* Chain up to the parent class */
    G_OBJECT_CLASS (gqr_solver_parent_class)->finalize(gobject);
}

static void gqr_solver_class_init(GqrSolverClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = gqr_solver_finalize;

    g_type_class_add_private(klass, sizeof(GqrSolverPrivate));
}

/*
 * Methods
 */

GqrSolver *gqr_solver_new(GqrCalculus *calculus) {
    GqrSolver *solver = (GqrSolver *)g_object_new(GQR_TYPE_SOLVER, NULL);
    solver->priv->solver = new GQR_Solver(*gqr_calculus_get_calculus(calculus));
    if (solver->priv->solver) {
        return solver;
    } else {
        g_object_unref(solver);
        return 0;
    }
}

void gqr_solver_unref(GqrSolver *self) {
    g_object_unref(self);
}

// chain instance to a specific tractable subclass (returns true on success)
bool gqr_solver_set_tractable_subclass(GqrSolver *self, char *algFilename) {
    return self->priv->solver->set_tractable_subclass(std::string(algFilename));
}

// return false if revised CSP contains empty relations
bool gqr_solver_enforce_algebraic_closure(GqrSolver *self, GqrCsp *i) {
    GQR_CSP *in = gqr_csp_get_csp(i);
    return self->priv->solver->enforce_algebraic_closure(*in);
}

// calculate and return an a-closed sub-csp (atomic or tractable)
GqrCsp* gqr_solver_get_scenario(GqrSolver *self, GqrCsp *input) {
    GQR_CSP *in = gqr_csp_get_csp(input);
    GQR_CSP *out = self->priv->solver->get_scenario(*in);
    if (out) {
        GqrCsp *result = gqr_csp_new_from_csp(out);
        return result;
    } else {
        return 0;
    }
}



GQR_Solver *gqr_solver_get_solver(GqrSolver *self) {
    return self->priv->solver;
}

