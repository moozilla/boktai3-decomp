/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_atan.c; see docs/LIBM.md. */
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
