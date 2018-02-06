#ifndef REAL_H
#define REAL_H
#include <limits>
#include <cstddef>
#include <stdint.h>
#include <sys/cdefs.h>
#include <string>
#include <sys/types.h>
#include "pconfig.h"

#ifdef REAL_FLOAT
typedef float real;
#else
#ifdef REAL_DOUBLE
typedef double real;
#else
#error need define REAL_FLOAT or REAL_DOUBLE
#endif

#endif


#endif // REAL_H
