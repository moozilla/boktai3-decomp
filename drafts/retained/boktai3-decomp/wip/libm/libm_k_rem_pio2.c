/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 k_rem_pio2.c; see docs/LIBM.md. */
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

extern const int gUnk_08603178[];
#define init_jk gUnk_08603178

extern const double gUnk_08603188[];
#define PIo2 gUnk_08603188

#define zero (0.0)
#define one (1.0)
#define two24 (1.67772160000000000000e+07)
#define twon24 (5.96046447753906250000e-08)

 int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec, const __int32_t *ipio2)
{
 __int32_t jz,jx,jv,jp,jk,carry,n,iq[20],i,j,k,m,q0,ih;
 double z,fw,f[20],fq[20],q[20];
 jk = init_jk[prec];
 jp = jk;
 jx = nx-1;
 jv = (e0-3)/24; if(jv<0) jv=0;
 q0 = e0-24*(jv+1);
 j = jv-jx; m = jx+jk;
 for(i=0;i<=m;i++,j++) f[i] = (j<0)? zero : (double) ipio2[j];
 for (i=0;i<=jk;i++) {
     for(j=0,fw=0.0;j<=jx;j++) fw += x[j]*f[jx+i-j]; q[i] = fw;
 }
 jz = jk;
recompute:
 for(i=0,j=jz,z=q[jz];j>0;i++,j--) {
     fw = (double)((__int32_t)(twon24* z));
     iq[i] = (__int32_t)(z-two24*fw);
     z = q[j-1]+fw;
 }
 z = scalbn(z,(int)q0);
 z -= 8.0*floor(z*0.125);
 n = (__int32_t) z;
 z -= (double)n;
 ih = 0;
 if(q0>0) {
     i = (iq[jz-1]>>(24-q0)); n += i;
     iq[jz-1] -= i<<(24-q0);
     ih = iq[jz-1]>>(23-q0);
 }
 else if(q0==0) ih = iq[jz-1]>>23;
 else if(z>=0.5) ih=2;
 if(ih>0) {
     n += 1; carry = 0;
     for(i=0;i<jz ;i++) {
  j = iq[i];
  if(carry==0) {
      if(j!=0) {
   carry = 1; iq[i] = 0x1000000- j;
      }
  } else iq[i] = 0xffffff - j;
     }
     if(q0>0) {
         switch(q0) {
         case 1:
         iq[jz-1] &= 0x7fffff; break;
      case 2:
         iq[jz-1] &= 0x3fffff; break;
         }
     }
     if(ih==2) {
  z = one - z;
  if(carry!=0) z -= scalbn(one,(int)q0);
     }
 }
 if(z==zero) {
     j = 0;
     for (i=jz-1;i>=jk;i--) j |= iq[i];
     if(j==0) {
  for(k=1;iq[jk-k]==0;k++);
  for(i=jz+1;i<=jz+k;i++) {
      f[jx+i] = (double) ipio2[jv+i];
      for(j=0,fw=0.0;j<=jx;j++) fw += x[j]*f[jx+i-j];
      q[i] = fw;
  }
  jz += k;
  goto recompute;
     }
 }
 if(z==0.0) {
     jz -= 1; q0 -= 24;
     while(iq[jz]==0) { jz--; q0-=24;}
 } else {
     z = scalbn(z,-(int)q0);
     if(z>=two24) {
  fw = (double)((__int32_t)(twon24*z));
  iq[jz] = (__int32_t)(z-two24*fw);
  jz += 1; q0 += 24;
  iq[jz] = (__int32_t) fw;
     } else iq[jz] = (__int32_t) z ;
 }
 fw = scalbn(one,(int)q0);
 for(i=jz;i>=0;i--) {
     q[i] = fw*(double)iq[i]; fw*=twon24;
 }
 for(i=jz;i>=0;i--) {
     for(fw=0.0,k=0;k<=jp&&k<=jz-i;k++) fw += PIo2[k]*q[i+k];
     fq[jz-i] = fw;
 }
 switch(prec) {
     case 0:
  fw = 0.0;
  for (i=jz;i>=0;i--) fw += fq[i];
  y[0] = (ih==0)? fw: -fw;
  break;
     case 1:
     case 2:
  fw = 0.0;
  for (i=jz;i>=0;i--) fw += fq[i];
  y[0] = (ih==0)? fw: -fw;
  fw = fq[0]-fw;
  for (i=1;i<=jz;i++) fw += fq[i];
  y[1] = (ih==0)? fw: -fw;
  break;
     case 3:
  for (i=jz;i>0;i--) {
      fw = fq[i-1]+fq[i];
      fq[i] += fq[i-1]-fw;
      fq[i-1] = fw;
  }
  for (i=jz;i>1;i--) {
      fw = fq[i-1]+fq[i];
      fq[i] += fq[i-1]-fw;
      fq[i-1] = fw;
  }
  for (fw=0.0,i=jz;i>=2;i--) fw += fq[i];
  if(ih==0) {
      y[0] = fq[0]; y[1] = fq[1]; y[2] = fw;
  } else {
      y[0] = -fq[0]; y[1] = -fq[1]; y[2] = -fw;
  }
 }
 return n&7;
}
