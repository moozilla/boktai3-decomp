/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 e_rem_pio2.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define sin sub_0824B138
#define __kernel_sin sub_0824D1B8
#define __subdf3 sub_08249A14
#define __ieee754_rem_pio2 sub_0824C174
#define __kernel_cos sub_0824C7E4
#define __negdf2 sub_0824A260
#define cos sub_0824B070
#define fabs sub_0824B120
#define __kernel_rem_pio2 sub_0824C9E0
#define __divsi3 sub_08249274
#define __floatsidf sub_0824A170
#define __muldf3 sub_08249A4C
#define __adddf3 sub_082499E4
#define __fixdfsi sub_0824A1EC
#define scalbn sub_0824D8B8
#define floor sub_0824D6F8
#define __gedf2 sub_0824A08C
#define __eqdf2 sub_08249FA8
#define atan sub_0824ACFC
#define __gtdf2 sub_0824A040
#define __divdf3 sub_08249CF4
#define isnan sub_0824D884
#define tan sub_0824B1EC
#define __kernel_tan sub_0824D338
#define __ieee754_fmod sub_0824BEDC
#define __ieee754_acos sub_0824B49C
#define __ieee754_sqrt sub_0824C5E0
#define __ieee754_asin sub_0824BA04
#define fmod sub_0824B3CC
#define __fdlib_version gUnk_086032A8
#define matherr sub_0824D8A4
#define __errno sub_0824DA3C
#define copysign sub_0824DA14
#define __nedf2 sub_08249FF4
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef union { double value; struct { __uint32_t msw, lsw; } parts; } ieee_double_shape_type;
typedef union { float value; __uint32_t word; } ieee_float_shape_type;
struct exception { int type; char *name; double arg1, arg2, retval; int err; };
enum __fdlibm_version { __fdlibm_ieee=-1, __fdlibm_svid, __fdlibm_xopen, __fdlibm_posix };
extern const enum __fdlibm_version __fdlib_version;
extern int matherr(struct exception *);
extern double atan (double);
extern double cos (double);
extern double sin (double);
extern double tan (double);
extern double tanh (double);
extern double frexp (double, int *);
extern double modf (double, double *);
extern double ceil (double);
extern double fabs (double);
extern double floor (double);
extern double acos (double);
extern double asin (double);
extern double atan2 (double, double);
extern double cosh (double);
extern double sinh (double);
extern double exp (double);
extern double ldexp (double, int);
extern double log (double);
extern double log10 (double);
extern double pow (double, double);
extern double sqrt (double);
extern double fmod (double, double);
extern double infinity (void);
extern double nan (void);
extern int isnan (double);
extern int isinf (double);
extern int finite (double);
extern double copysign (double, double);
extern int ilogb (double);
extern double asinh (double);
extern double cbrt (double);
extern double nextafter (double, double);
extern double rint (double);
extern double scalbn (double, int);
extern double log1p (double);
extern double expm1 (double);
extern double acosh (double);
extern double atanh (double);
extern double remainder (double, double);
extern double gamma (double);
extern double gamma_r (double, int *);
extern double lgamma (double);
extern double lgamma_r (double, int *);
extern double erf (double);
extern double erfc (double);
extern double y0 (double);
extern double y1 (double);
extern double yn (int, double);
extern double j0 (double);
extern double j1 (double);
extern double jn (int, double);
extern double hypot (double, double);
extern double drem (double, double);
extern int isnanf (float);
extern int isinff (float);
extern int finitef (float);
extern int ilogbf (float);
extern double logb (double);
extern double scalb (double, double);
extern double significand (double);
extern double __ieee754_sqrt (double);
extern double __ieee754_acos (double);
extern double __ieee754_acosh (double);
extern double __ieee754_log (double);
extern double __ieee754_atanh (double);
extern double __ieee754_asin (double);
extern double __ieee754_atan2 (double,double);
extern double __ieee754_exp (double);
extern double __ieee754_cosh (double);
extern double __ieee754_fmod (double,double);
extern double __ieee754_pow (double,double);
extern double __ieee754_lgamma_r (double,int *);
extern double __ieee754_gamma_r (double,int *);
extern double __ieee754_log10 (double);
extern double __ieee754_sinh (double);
extern double __ieee754_hypot (double,double);
extern double __ieee754_j0 (double);
extern double __ieee754_j1 (double);
extern double __ieee754_y0 (double);
extern double __ieee754_y1 (double);
extern double __ieee754_jn (int,double);
extern double __ieee754_yn (int,double);
extern double __ieee754_remainder (double,double);
extern __int32_t __ieee754_rem_pio2 (double,double*);
extern double __ieee754_scalb (double,double);
extern double __kernel_standard (double,double,int);
extern double __kernel_sin (double,double,int);
extern double __kernel_cos (double,double);
extern double __kernel_tan (double,double,int);
extern int __kernel_rem_pio2 (double*,double*,int,int,int,const __int32_t*);
extern __int32_t __ieee754_rem_pio2f (float,float*);
extern int __kernel_rem_pio2f (float*,float*,int,int,int,const __int32_t*);

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
