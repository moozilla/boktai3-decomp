/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_isnan.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define isnan sub_0824D884
#include "libm_compat.h"

 int isnan(double x)
{
 __int32_t hx,lx;
 do { ieee_double_shape_type ew_u; ew_u.value = (x); (hx) = ew_u.parts.msw; (lx) = ew_u.parts.lsw; } while (0);
 hx &= 0x7fffffff;
 hx |= (__uint32_t)(lx|(-lx))>>31;
 hx = 0x7ff00000 - hx;
 return (int)(((__uint32_t)(hx))>>31);
}
