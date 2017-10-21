#ifndef GQR_H
#define GQR_H

/* Compile time setting */
#if HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

/* GQR specific settings */

#include <string>

#ifdef GQR_DEFAULT_DATA_DIR
static const std::string gqr_default_data_dir = GQR_DEFAULT_DATA_DIR;
#else
static const std::string gqr_default_data_dir = "";
#endif

// set additional relation data type
#ifdef RELATION_SIZE
// RELATION_SIZE needs to be const numeric value (preprocessor does not evaluate sizeof())
// less than 8 is pointless
#if RELATION_SIZE > 8
#if RELATION_SIZE <= 16
#define USER_RELATION_TYPE uint16_t
#define USER_RELATION_TYPE_SIZE 1
#else
#define USER_RELATION_TYPE size_t
#define USER_RELATION_TYPE_SIZE ( (size_t) (RELATION_SIZE + sizeof(size_t)*8 - 1) / (sizeof(size_t)*8) )
#endif // RELATION_SIZE <= 16
#endif // RELATION_SIZE > 8

#endif // #ifdef RELATION_SIZE

#endif // GQR_H
