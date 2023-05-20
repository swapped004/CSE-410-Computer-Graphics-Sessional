#include "lib/precompiled.h"

#ifndef ENABLE_CHECKS
# ifdef NDEBUG
#  define ENABLE_CHECKS 0
# else
#  define ENABLE_CHECKS 1
# endif
#endif

#if ENABLE_CHECKS
#define VERIFY(expr) debug_assert(expr)
#else
#define VERIFY(expr) (void)(expr)
#endif
