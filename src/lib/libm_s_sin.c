/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_sin.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define sin sub_0824B138
#include "libm_compat.h"

 double sin(double x)
{
 double y[2],z=0.0;
 __int32_t n,ix;
 do { ieee_double_shape_type gh_u; gh_u.value = (x); (ix) = gh_u.parts.msw; } while (0);
 ix &= 0x7fffffff;
 if(ix <= 0x3fe921fb) return __kernel_sin(x,z,0);
 else if (ix>=0x7ff00000) return x-x;
 else {
     n = __ieee754_rem_pio2(x,y);
     switch(n&3) {
  case 0: return __kernel_sin(y[0],y[1],1);
  case 1: return __kernel_cos(y[0],y[1]);
  case 2: return -__kernel_sin(y[0],y[1],1);
  default:
   return -__kernel_cos(y[0],y[1]);
     }
 }
}
