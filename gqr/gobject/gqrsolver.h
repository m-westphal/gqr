/*
 * gqrsolver.h
 *
 *  Created on: Aug 16, 2011
 *      Author: thomas
 */

#ifndef GQRSOLVER_H_
#define GQRSOLVER_H_


#include <glib-object.h>
#include "gqrcalculus.h"
#include "gqrcsp.h"

#ifndef __cplusplus

#include <stdbool.h>

#else

#include "gqr_wrap.h"


extern "C" {
#endif

/*
 * Typedef common macros
 */
#define GQR_TYPE_SOLVER (gqr_solver_get_type())
#define GQR_SOLVER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GQR_TYPE_SOLVER, GqrSolver))
#define GQR_IS_SOLVER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GQR_TYPE_SOLVER))
#define GQR_SOLVER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GQR_TYPE_SOLVER, GqrSolverClass))
#define GQR_IS_SOLVER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GQR_TYPE_SOLVER))
#define GQR_SOLVER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GQR_TYPE_SOLVER, GqrSolverClass))

typedef struct _GqrSolver        GqrSolver;
typedef struct _GqrSolverClass   GqrSolverClass;
typedef struct _GqrSolverPrivate GqrSolverPrivate;

struct _GqrSolver
{
  GObject parent_instance;

  /* instance members */
  GqrSolverPrivate *priv;
};

struct _GqrSolverClass
{
  GObjectClass parent_class;

  /* class members */
};


/*
 * Methods
 */

/* create new solver */
GqrSolver *gqr_solver_new(GqrCalculus *calculus);

/* delete solver */
void gqr_solver_unref(GqrSolver *self);

/* chain instance to a specific tractable subclass (returns true on success) */
bool gqr_solver_set_tractable_subclass(GqrSolver *self, char *algFilename);

/* enforce algebraic closure on csp: aborts and returns false once an empty
   relation has been inferred; returns true if network is now a-closed. */
bool gqr_solver_enforce_algebraic_closure(GqrSolver *self, GqrCsp *i);

/* calculate and return an a-closed sub-csp (atomic or tractable relations;
   depending on whether a tractable subclass has been loaded) */
GqrCsp* gqr_solver_get_scenario(GqrSolver *self, GqrCsp *input);



#ifdef __cplusplus

}

// returns C++ type, only valid in C++ mode!
GQR_Solver *gqr_solver_get_solver(GqrSolver *self);

#endif



#endif /* GQRSOLVER_H_ */
