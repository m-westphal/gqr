/*
 * gqrcalculus.h
 *
 *  Created on: Aug 16, 2011
 *      Author: thomas
 */

#ifndef GQRCALCULUS_H_
#define GQRCALCULUS_H_

#include <glib-object.h>

#ifndef __cplusplus

#include <stdbool.h>

#else

#include "gqr_wrap.h"

extern "C" {
#endif


/*
 * Typedef common macros
 */
#define GQR_TYPE_CALCULUS (gqr_calculus_get_type())
#define GQR_CALCULUS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GQR_TYPE_CALCULUS, GqrCalculus))
#define GQR_IS_CALCULUS(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GQR_TYPE_CALCULUS))
#define GQR_CALCULUS_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GQR_TYPE_CALCULUS, GqrCalculusClass))
#define GQR_IS_CALCULUS_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GQR_TYPE_CALCULUS))
#define GQR_CALCULUS_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GQR_TYPE_CALCULUS, GqrCalculusClass))

typedef struct _GqrCalculus        GqrCalculus;
typedef struct _GqrCalculusClass   GqrCalculusClass;
typedef struct _GqrCalculusPrivate GqrCalculusPrivate;

struct _GqrCalculus
{
  GObject parent_instance;

  /* instance members */
  GqrCalculusPrivate *priv;
};

struct _GqrCalculusClass
{
  GObjectClass parent_class;

  /* class members */
};

/*
 * Method definitions.
 */

/* initialize a new calculus; requires a valid "name".spec in "data_dir" */
GqrCalculus *gqr_calculus_new(char *name, char *data_dir);

/* delete calculus */
void gqr_calculus_unref(GqrCalculus *self);

/* returns the universal relations, i.e., a list of all base relations */
char *gqr_calculus_get_base_relations(GqrCalculus *self);

/* calculate composition */
char *gqr_calculus_get_composition(GqrCalculus *self, char *a_r, char *b_r);

/* calculate converse */
char *gqr_calculus_get_converse(GqrCalculus *self, char *r_raw);


#ifdef __cplusplus

}

// returns C++ type, only valid in C++ mode!
GQR_Calculus *gqr_calculus_get_calculus(GqrCalculus *self);

#endif


#endif /* GQRCALCULUS_H_ */
