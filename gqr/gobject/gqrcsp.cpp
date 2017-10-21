/*
 * gqrcsp.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: thomas
 */

#include <cstring>
#include <stdlib.h>

#include "gqrcsp.h"
#include "gqrcalculus.h"

extern "C" {
G_DEFINE_TYPE(GqrCsp, gqr_csp, G_TYPE_OBJECT)
}

/*
 * Private member stuff
 */
#define GQR_CSP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GQR_TYPE_CSP, GqrCspPrivate))

struct _GqrCspPrivate
{
    GQR_CSP *csp;
};


/*
 * Constructor
 */
static void gqr_csp_init(GqrCsp *self)
{
    self->priv = GQR_CSP_GET_PRIVATE(self);
    self->priv->csp = 0;
}

/*
 * Destructor
 */
static void gqr_csp_finalize(GObject *gobject)
{
    GqrCsp *self = GQR_CSP_C(gobject);

    delete self->priv->csp;

    /* Chain up to the parent class */
    G_OBJECT_CLASS (gqr_csp_parent_class)->finalize(gobject);
}

//static void gqr_csp_constructed(GObject *gobject)
//{
//    GqrCsp *self = GQR_CSP_C(gobject);
//
//    gint size;
//    GqrCalculus *calculus;
//
//    g_object_get(gobject, "size", &size, "calculus", &calculus, NULL);
//    self->priv->csp = new GQR_CSP(int(size),
//	    *gqr_calculus_get_calculus(calculus));
//}

static void gqr_csp_class_init(GqrCspClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = gqr_csp_finalize;
//    gobject_class->set_property = gqr_csp_set_property;
//    gobject_class->get_property = gqr_csp_get_property;
//    gobject_class->constructed = gqr_csp_constructed;

//    /* Add properties */
//    GParamSpec *pspec;
//    pspec = g_param_spec_int("size", "Size of the CSP", NULL, 0, 1000, 1,
//	    GParamFlags(G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
//    g_object_class_install_property(gobject_class, PROP_CSP_SIZE, pspec);
//
//    pspec = g_param_spec_object("calculus", "Calculus", NULL, GQR_TYPE_CALCULUS,
//	    GParamFlags(G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
//    g_object_class_install_property(gobject_class, PROP_CSP_CALCULUS, pspec);

    g_type_class_add_private(klass, sizeof(GqrCspPrivate));
}

/*
 * Methods
 */

GqrCsp *gqr_csp_new(int size, GqrCalculus *calculus)
{
    GqrCsp *csp = (GqrCsp *)g_object_new(GQR_TYPE_CSP, NULL);

    csp->priv->csp = new GQR_CSP(size, *gqr_calculus_get_calculus(calculus));
    if (csp->priv->csp)
    {
        return csp;
    } else
    {
        g_object_unref(csp);
        return 0;
    }
}

void gqr_csp_unref(GqrCsp *self) {
    g_object_unref(self);
}

char *gqr_csp_get_name(GqrCsp *self)
{
    const std::string name = self->priv->csp->getName();
    char const *s = name.c_str();
    size_t slen = strlen(s);
    char *result = (char *) malloc(slen + 1);
    std::strcpy(result, s);
    return result;
}

void gqr_csp_set_name(GqrCsp *self, char *s)
{
    self->priv->csp->setName(std::string(s));
}

int gqr_csp_get_size(GqrCsp *self)
{
    return self->priv->csp->getSize();
}

bool gqr_csp_check_equals(GqrCsp *self, GqrCsp *other)
{
    GQR_CSP *b = other->priv->csp;
    return self->priv->csp->check_equals(*b);
}

bool gqr_csp_check_refines(GqrCsp *self, GqrCsp *other)
{
    GQR_CSP *b = other->priv->csp;
    return self->priv->csp->check_refines(*b);
}

// add a constraint
bool gqr_csp_add_constraint(GqrCsp *self, int i, int j, char *relation)
{
    return self->priv->csp->add_constraint(i, j, std::string(relation));
}

// set a constraint (i.e. replace)
bool gqr_csp_set_constraint(GqrCsp *self, int i, int j, char *relation)
{
    return self->priv->csp->set_constraint(i, j, std::string(relation));
}
// read constraint
char *gqr_csp_get_constraint(GqrCsp *self, int i, int j)
{
    const std::string c = self->priv->csp->get_constraint(i, j);
    char const *s = c.c_str();
    size_t slen = strlen(s);
    char *result = (char *) malloc(slen + 1);
    std::strcpy(result, s);
    return result;
}

GQR_CSP *gqr_csp_get_csp(GqrCsp *self)
{
    return self->priv->csp;
}

GqrCsp *gqr_csp_new_from_csp(GQR_CSP *csp)
{
    if (csp)
    {
        GqrCsp *_csp = (GqrCsp *)g_object_new(GQR_TYPE_CSP, NULL);
        _csp->priv->csp = csp;
        return _csp;
    } else
    {
        return 0;
    }
}
