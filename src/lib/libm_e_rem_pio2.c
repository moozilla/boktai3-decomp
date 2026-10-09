/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 e_rem_pio2.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define __ieee754_rem_pio2 sub_0824C174
#include "libm_compat.h"

extern const __int32_t gUnk_08602F58[];
#define two_over_pi gUnk_08602F58

extern const __int32_t gUnk_08603060[];
#define npio2_hw gUnk_08603060

#define zero (0.00000000000000000000e+00)
#define half (5.00000000000000000000e-01)
#define two24 (1.67772160000000000000e+07)
#define invpio2 (6.36619772367581382433e-01)
#define pio2_1 (1.57079632673412561417e+00)
#define pio2_1t (6.07710050650619224932e-11)
#define pio2_2 (6.07710050630396597660e-11)
#define pio2_2t (2.02226624879595063154e-21)
#define pio2_3 (2.02226624871116645580e-21)
#define pio2_3t (8.47842766036889956997e-32)

 __int32_t __ieee754_rem_pio2(double x, double *y)
{
 double z,w,t,r,fn;
 double tx[3];
 __int32_t i,j,n,ix,hx;
 int e0,nx;
 __uint32_t low;
 do { ieee_double_shape_type gh_u; gh_u.value = (x); (hx) = gh_u.parts.msw; } while (0);
 ix = hx&0x7fffffff;
 if(ix<=0x3fe921fb)
     {y[0] = x; y[1] = 0; return 0;}
 if(ix<0x4002d97c) {
     if(hx>0) {
  z = x - pio2_1;
  if(ix!=0x3ff921fb) {
      y[0] = z - pio2_1t;
      y[1] = (z-y[0])-pio2_1t;
  } else {
      z -= pio2_2;
      y[0] = z - pio2_2t;
      y[1] = (z-y[0])-pio2_2t;
  }
  return 1;
     } else {
  z = x + pio2_1;
  if(ix!=0x3ff921fb) {
      y[0] = z + pio2_1t;
      y[1] = (z-y[0])+pio2_1t;
  } else {
      z += pio2_2;
      y[0] = z + pio2_2t;
      y[1] = (z-y[0])+pio2_2t;
  }
  return -1;
     }
 }
 if(ix<=0x413921fb) {
     t = fabs(x);
     n = (__int32_t) (t*invpio2+half);
     fn = (double)n;
     r = t-fn*pio2_1;
     w = fn*pio2_1t;
     if(n<32&&ix!=npio2_hw[n-1]) {
  y[0] = r-w;
     } else {
         __uint32_t high;
         j = ix>>20;
         y[0] = r-w;
  do { ieee_double_shape_type gh_u; gh_u.value = (y[0]); (high) = gh_u.parts.msw; } while (0);
         i = j-((high>>20)&0x7ff);
         if(i>16) {
      t = r;
      w = fn*pio2_2;
      r = t-w;
      w = fn*pio2_2t-((t-r)-w);
      y[0] = r-w;
      do { ieee_double_shape_type gh_u; gh_u.value = (y[0]); (high) = gh_u.parts.msw; } while (0);
      i = j-((high>>20)&0x7ff);
      if(i>49) {
       t = r;
       w = fn*pio2_3;
       r = t-w;
       w = fn*pio2_3t-((t-r)-w);
       y[0] = r-w;
      }
  }
     }
     y[1] = (r-y[0])-w;
     if(hx<0) {y[0] = -y[0]; y[1] = -y[1]; return -n;}
     else return n;
 }
 if(ix>=0x7ff00000) {
     y[0]=y[1]=x-x; return 0;
 }
 do { ieee_double_shape_type gl_u; gl_u.value = (x); (low) = gl_u.parts.lsw; } while (0);
 do { ieee_double_shape_type sl_u; sl_u.value = (z); sl_u.parts.lsw = (low); (z) = sl_u.value; } while (0);
 e0 = (int)((ix>>20)-1046);
 do { ieee_double_shape_type sh_u; sh_u.value = (z); sh_u.parts.msw = (ix - ((__int32_t)e0<<20)); (z) = sh_u.value; } while (0);
 for(i=0;i<2;i++) {
  tx[i] = (double)((__int32_t)(z));
  z = (z-tx[i])*two24;
 }
 tx[2] = z;
 nx = 3;
 while(tx[nx-1]==zero) nx--;
 n = __kernel_rem_pio2(tx,y,e0,nx,2,two_over_pi);
 if(hx<0) {y[0] = -y[0]; y[1] = -y[1]; return -n;}
 return n;
}
