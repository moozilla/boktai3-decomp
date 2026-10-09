/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 k_cos.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define __kernel_cos sub_0824C7E4
#include "libm_compat.h"

#define one (1.00000000000000000000e+00)
#define C1 (4.16666666666666019037e-02)
#define C2 (-1.38888888888741095749e-03)
#define C3 (2.48015872894767294178e-05)
#define C4 (-2.75573143513906633035e-07)
#define C5 (2.08757232129817482790e-09)
#define C6 (-1.13596475577881948265e-11)

 double __kernel_cos(double x, double y)
{
 double a,hz,z,r,qx;
 __int32_t ix;
 do { ieee_double_shape_type gh_u; gh_u.value = (x); (ix) = gh_u.parts.msw; } while (0);
 ix &= 0x7fffffff;
 if(ix<0x3e400000) {
     if(((int)x)==0) return one;
 }
 z = x*x;
 r = z*(C1+z*(C2+z*(C3+z*(C4+z*(C5+z*C6)))));
 if(ix < 0x3FD33333)
     return one - (0.5*z - (z*r - x*y));
 else {
     if(ix > 0x3fe90000) {
  qx = 0.28125;
     } else {
         do { ieee_double_shape_type iw_u; iw_u.parts.msw = (ix-0x00200000); iw_u.parts.lsw = (0); (qx) = iw_u.value; } while (0);
     }
     hz = 0.5*z-qx;
     a = one-qx;
     return a - (hz - (z*r-x*y));
 }
}
