/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_atan.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define atan sub_0824ACFC
#include "libm_compat.h"

extern const double gUnk_08602D98[];
#define atanhi gUnk_08602D98

extern const double gUnk_08602DB8[];
#define atanlo gUnk_08602DB8

extern const double gUnk_08602DD8[];
#define aT gUnk_08602DD8

 #define one (1.0)
#define huge (1.0e300)

 double atan(double x)
{
 double w,s1,s2,z;
 __int32_t ix,hx,id;
 do { ieee_double_shape_type gh_u; gh_u.value = (x); (hx) = gh_u.parts.msw; } while (0);
 ix = hx&0x7fffffff;
 if(ix>=0x44100000) {
     __uint32_t low;
     do { ieee_double_shape_type gl_u; gl_u.value = (x); (low) = gl_u.parts.lsw; } while (0);
     if(ix>0x7ff00000||
  (ix==0x7ff00000&&(low!=0)))
  return x+x;
     if(hx>0) return atanhi[3]+atanlo[3];
     else return -atanhi[3]-atanlo[3];
 } if (ix < 0x3fdc0000) {
     if (ix < 0x3e200000) {
  if(huge+x>one) return x;
     }
     id = -1;
 } else {
 x = fabs(x);
 if (ix < 0x3ff30000) {
     if (ix < 0x3fe60000) {
  id = 0; x = (2.0*x-one)/(2.0+x);
     } else {
  id = 1; x = (x-one)/(x+one);
     }
 } else {
     if (ix < 0x40038000) {
  id = 2; x = (x-1.5)/(one+1.5*x);
     } else {
  id = 3; x = -1.0/x;
     }
 }}
 z = x*x;
 w = z*z;
 s1 = z*(aT[0]+w*(aT[2]+w*(aT[4]+w*(aT[6]+w*(aT[8]+w*aT[10])))));
 s2 = w*(aT[1]+w*(aT[3]+w*(aT[5]+w*(aT[7]+w*aT[9]))));
 if (id<0) return x - x*(s1+s2);
 else {
     z = atanhi[id] - ((x*(s1+s2) - atanlo[id]) - x);
     return (hx<0)? -z:z;
 }
}
