/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_tan.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#include "libm_compat.h"

 double tan(double x)
{
 double y[2],z=0.0;
 __int32_t n,ix;
 GET_HIGH_WORD(ix, x);
 ix &= 0x7fffffff;
 if(ix <= 0x3fe921fb) return __kernel_tan(x,z,1);
 else if (ix>=0x7ff00000) return x-x;
 else {
     n = __ieee754_rem_pio2(x,y);
     return __kernel_tan(y[0],y[1],1-((n&1)<<1));
 }
}
