/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 e_asin.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define __ieee754_asin sub_0824BA04
#include "libm_compat.h"

#define one (1.00000000000000000000e+00)
#define huge (1.000e+300)
#define pio2_hi (1.57079632679489655800e+00)
#define pio2_lo (6.12323399573676603587e-17)
#define pio4_hi (7.85398163397448278999e-01)
#define pS0 (1.66666666666666657415e-01)
#define pS1 (-3.25565818622400915405e-01)
#define pS2 (2.01212532134862925881e-01)
#define pS3 (-4.00555345006794114027e-02)
#define pS4 (7.91534994289814532176e-04)
#define pS5 (3.47933107596021167570e-05)
#define qS1 (-2.40339491173441421878e+00)
#define qS2 (2.02094576023350569471e+00)
#define qS3 (-6.88283971605453293030e-01)
#define qS4 (7.70381505559019352791e-02)

 double __ieee754_asin(double x)
{
 double t,w,p,q,c,r,s;
 __int32_t hx,ix;
 do { ieee_double_shape_type gh_u; gh_u.value = (x); (hx) = gh_u.parts.msw; } while (0);
 ix = hx&0x7fffffff;
 if(ix>= 0x3ff00000) {
     __uint32_t lx;
     do { ieee_double_shape_type gl_u; gl_u.value = (x); (lx) = gl_u.parts.lsw; } while (0);
     if(((ix-0x3ff00000)|lx)==0)
  return x*pio2_hi+x*pio2_lo;
     return (x-x)/(x-x);
 } else if (ix<0x3fe00000) {
     if(ix<0x3e400000) {
  if(huge+x>one) return x;
     } else
  t = x*x;
  p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
  q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
  w = p/q;
  return x+x*w;
 }
 w = one-fabs(x);
 t = w*0.5;
 p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
 q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
 s = __ieee754_sqrt(t);
 if(ix>=0x3FEF3333) {
     w = p/q;
     t = pio2_hi-(2.0*(s+s*w)-pio2_lo);
 } else {
     w = s;
     do { ieee_double_shape_type sl_u; sl_u.value = (w); sl_u.parts.lsw = (0); (w) = sl_u.value; } while (0);
     c = (t-w*w)/(s+w);
     r = p/q;
     p = 2.0*s*r-(pio2_lo-2.0*c);
     q = pio4_hi-2.0*w;
     t = pio4_hi-(p-q);
 }
 if(hx>0) return t; else return -t;
}
