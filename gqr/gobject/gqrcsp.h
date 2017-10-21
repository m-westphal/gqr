/*
 * gqrcsp.h
 *
 *  Created on: Aug 16, 2011
 *      Author: thomas
 */

#ifndef GQRCSP_H_
#define GQRCSP_H_

#include <glib-object.h>
#include <gobject/gqrcalculus.h>


#ifndef __cplusplus

#include <stdbool.h>

#else

#include "gqr_wrap.h"


extern "C" {
#endif

/*
 * Typedef common macros
 */
#define GQR_TYPE_CSP (gqr_csp_get_type())
#define GQR_CSP_C(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GQR_TYPE_CSP, GqrCsp))
#define GQR_IS_CSP(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GQR_TYPE_CSP))
#define GQR_CSP_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GQR_TYPE_CSP, GqrCspClass))
#define GQR_IS_CSP_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GQR_TYPE_CSP))
#define GQR_CSP_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GQR_TYPE_CSP, GqrCspClass))

typedef struct _GqrCsp        GqrCsp;
typedef struct _GqrCspClass   GqrCspClass;
typedef struct _GqrCspPrivate GqrCspPrivate;

struct _GqrCsp
{
  GObject parent_instance;

  /* instance members */
  GqrCspPrivate *priv;
};

struct _GqrCspClass
{
  GObjectClass parent_class;

  /* class members */
};


/* create new csp with "size" variables and relations from "calculus".
   the network is initially unconstrained */
GqrCsp *gqr_csp_new(int size, GqrCalculus *calculus);

/* delete csp */
void gqr_csp_unref(GqrCsp *self);

/* set a name for the csp */
void gqr_csp_set_name(GqrCsp *self, char *s);

/* read name of csp */
char *gqr_csp_get_name(GqrCsp *self);

/* read size of csp */
int gqr_csp_get_size(GqrCsp *self);

/* test if all constraints in "self" are exactly the same as in "other" */
bool gqr_csp_check_equals(GqrCsp *self, GqrCsp *other);

/* test if "self" is more refined than "other", i.e., "self" does not equal
   "other" and all constraints are subsets of those in "other". */
bool gqr_csp_check_refines(GqrCsp *self, GqrCsp *other);

/* add a constraint to the csp; if (i,j) is already constrained the constraint
   will be tightened */
bool gqr_csp_add_constraint(GqrCsp *self, int i, int j, char *relation);

/* set a constraint, i.e., replace any previous constraint on (i,j) */
bool gqr_csp_set_constraint(GqrCsp *self, int i, int j, char *relation);

/* read constraint from csp */
char *gqr_csp_get_constraint(GqrCsp *self, int i, int j);





#ifdef __cplusplus

}

// returns C++ type, only valid in C++ mode!
GQR_CSP *gqr_csp_get_csp(GqrCsp *self);

// wrap an existing C++ instance to GType CSP
GqrCsp *gqr_csp_new_from_csp(GQR_CSP *csp);


#endif

#endif /* GQRCSP_H_ */
