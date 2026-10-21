/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_copysign.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#include "libm_compat.h"

 double copysign(double x, double y)
{
 __uint32_t hx,hy;
 GET_HIGH_WORD(hx, x);
 GET_HIGH_WORD(hy, y);
 SET_HIGH_WORD(x, (hx&0x7fffffff)|(hy&0x80000000));
        return x;
}

