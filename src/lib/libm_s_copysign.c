/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_copysign.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define copysign sub_0824DA14
#include "libm_compat.h"

 double copysign(double x, double y)
{
 __uint32_t hx,hy;
 do { ieee_double_shape_type gh_u; gh_u.value = (x); (hx) = gh_u.parts.msw; } while (0);
 do { ieee_double_shape_type gh_u; gh_u.value = (y); (hy) = gh_u.parts.msw; } while (0);
 do { ieee_double_shape_type sh_u; sh_u.value = (x); sh_u.parts.msw = ((hx&0x7fffffff)|(hy&0x80000000)); (x) = sh_u.value; } while (0);
        return x;
}

