/*
 * gqrcalculus.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: thomas
 */

#include <cstring>
#include "gqrcalculus.h"

#include <stdlib.h>


extern "C" {
G_DEFINE_TYPE(GqrCalculus, gqr_calculus, G_TYPE_OBJECT)
}

/*
 * Private member stuff
 */
#define GQR_CALCULUS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GQR_TYPE_CALCULUS, GqrCalculusPrivate))

struct _GqrCalculusPrivate
{
    GQR_Calculus *calculus;
};

/*
 * Constructor
 */
static void gqr_calculus_init(GqrCalculus *self)
{
    self->priv = GQR_CALCULUS_GET_PRIVATE(self);
    self->priv->calculus = 0;
}

/*
 * Destructor
 */
static void gqr_calculus_finalize(GObject *gobject)
{
    GqrCalculus *self = GQR_CALCULUS(gobject);

    delete self->priv->calculus;

    /* Chain up to the parent class */
    G_OBJECT_CLASS (gqr_calculus_parent_class)->finalize(gobject);
}

static void gqr_calculus_class_init(GqrCalculusClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = gqr_calculus_finalize;
    g_type_class_add_private(klass, sizeof(GqrCalculusPrivate));
}

/*
 * Methods
 */

GqrCalculus *gqr_calculus_new(char *name, char *data_dir)
{
    GqrCalculus *calculus = (GqrCalculus *)g_object_new(GQR_TYPE_CALCULUS, NULL);
    calculus->priv->calculus = new GQR_Calculus;
    if (calculus->priv->calculus)
    {
        if (calculus->priv->calculus->set_calculus(std::string(name),
                std::string(data_dir))) {
            return calculus;
        }
    }

    g_object_unref(calculus);
    return 0;
}

void gqr_calculus_unref(GqrCalculus *self) {
    g_object_unref(self);
}

// returns the universal relations
char *gqr_calculus_get_base_relations(GqrCalculus *self)
{
    const std::string br = self->priv->calculus->get_base_relations();
    char const *s = br.c_str();
    size_t slen = strlen(s);
    char *result = (char *) malloc(slen + 1);
    std::strcpy(result, s);
    return result;
}

// calculate composition
char *gqr_calculus_get_composition(GqrCalculus *self, char *a_r, char *b_r)
{
    const std::string cmp = self->priv->calculus->get_composition(
            std::string(a_r), std::string(b_r));
    char const *s = cmp.c_str();
    size_t slen = strlen(s);
    char *result = (char *) malloc(slen + 1);
    std::strcpy(result, s);
    return result;
}

// calculate converse
char *gqr_calculus_get_converse(GqrCalculus *self, char *r_raw)
{
    const std::string cnv =
            self->priv->calculus->get_converse(std::string(r_raw));
    char const *s = cnv.c_str();
    size_t slen = strlen(s);
    char *result = (char *) malloc(slen + 1);
    std::strcpy(result, s);
    return result;
}


GQR_Calculus *gqr_calculus_get_calculus(GqrCalculus *self)
{
    return self->priv->calculus;
}

