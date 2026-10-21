/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Types, word layout and prototypes from newlib 1.8.2 fdlibm headers.
 * Address aliases are verified from fully linked B3 objects; docs/LIBM.md. */
#ifndef BOKTAI3_LIBM_COMPAT_H
#define BOKTAI3_LIBM_COMPAT_H
#define __fdlib_version gUnk_086032A8
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef union {
    double value;
    struct {
        __uint32_t msw, lsw;
    } parts;
} ieee_double_shape_type;
typedef union {
    float value;
    __uint32_t word;
} ieee_float_shape_type;
/* Word access macros from newlib's fdlibm.h (math_private.h). */
#define EXTRACT_WORDS(ix0, ix1, d) do { ieee_double_shape_type ew_u; ew_u.value = (d); (ix0) = ew_u.parts.msw; (ix1) = ew_u.parts.lsw; } while (0)
#define INSERT_WORDS(d, ix0, ix1) do { ieee_double_shape_type iw_u; iw_u.parts.msw = (ix0); iw_u.parts.lsw = (ix1); (d) = iw_u.value; } while (0)
#define GET_HIGH_WORD(i, d) do { ieee_double_shape_type gh_u; gh_u.value = (d); (i) = gh_u.parts.msw; } while (0)
#define GET_LOW_WORD(i, d) do { ieee_double_shape_type gl_u; gl_u.value = (d); (i) = gl_u.parts.lsw; } while (0)
#define SET_HIGH_WORD(d, v) do { ieee_double_shape_type sh_u; sh_u.value = (d); sh_u.parts.msw = (v); (d) = sh_u.value; } while (0)
#define SET_LOW_WORD(d, v) do { ieee_double_shape_type sl_u; sl_u.value = (d); sl_u.parts.lsw = (v); (d) = sl_u.value; } while (0)
#define GET_FLOAT_WORD(i, d) do { ieee_float_shape_type gf_u; gf_u.value = (d); (i) = gf_u.word; } while (0)
#define SET_FLOAT_WORD(d, i) do { ieee_float_shape_type sf_u; sf_u.word = (i); (d) = sf_u.value; } while (0)
struct exception {
    int type;
    char *name;
    double arg1, arg2, retval;
    int err;
};
enum __fdlibm_version { __fdlibm_ieee = -1, __fdlibm_svid, __fdlibm_xopen, __fdlibm_posix };
extern const enum __fdlibm_version __fdlib_version;
extern int matherr(struct exception *);
extern double atan(double);
extern double cos(double);
extern double sin(double);
extern double tan(double);
extern double tanh(double);
extern double frexp(double, int *);
extern double modf(double, double *);
extern double ceil(double);
extern double fabs(double);
extern double floor(double);
extern double acos(double);
extern double asin(double);
extern double atan2(double, double);
extern double cosh(double);
extern double sinh(double);
extern double exp(double);
extern double ldexp(double, int);
extern double log(double);
extern double log10(double);
extern double pow(double, double);
extern double sqrt(double);
extern double fmod(double, double);
extern double infinity(void);
extern double nan(void);
extern int isnan(double);
extern int isinf(double);
extern int finite(double);
extern double copysign(double, double);
extern int ilogb(double);
extern double asinh(double);
extern double cbrt(double);
extern double nextafter(double, double);
extern double rint(double);
extern double scalbn(double, int);
extern double log1p(double);
extern double expm1(double);
extern double acosh(double);
extern double atanh(double);
extern double remainder(double, double);
extern double gamma(double);
extern double gamma_r(double, int *);
extern double lgamma(double);
extern double lgamma_r(double, int *);
extern double erf(double);
extern double erfc(double);
extern double y0(double);
extern double y1(double);
extern double yn(int, double);
extern double j0(double);
extern double j1(double);
extern double jn(int, double);
extern double hypot(double, double);
extern double drem(double, double);
extern int isnanf(float);
extern int isinff(float);
extern int finitef(float);
extern int ilogbf(float);
extern double logb(double);
extern double scalb(double, double);
extern double significand(double);
extern double __ieee754_sqrt(double);
extern double __ieee754_acos(double);
extern double __ieee754_acosh(double);
extern double __ieee754_log(double);
extern double __ieee754_atanh(double);
extern double __ieee754_asin(double);
extern double __ieee754_atan2(double, double);
extern double __ieee754_exp(double);
extern double __ieee754_cosh(double);
extern double __ieee754_fmod(double, double);
extern double __ieee754_pow(double, double);
extern double __ieee754_lgamma_r(double, int *);
extern double __ieee754_gamma_r(double, int *);
extern double __ieee754_log10(double);
extern double __ieee754_sinh(double);
extern double __ieee754_hypot(double, double);
extern double __ieee754_j0(double);
extern double __ieee754_j1(double);
extern double __ieee754_y0(double);
extern double __ieee754_y1(double);
extern double __ieee754_jn(int, double);
extern double __ieee754_yn(int, double);
extern double __ieee754_remainder(double, double);
extern __int32_t __ieee754_rem_pio2(double, double *);
extern double __ieee754_scalb(double, double);
extern double __kernel_standard(double, double, int);
extern double __kernel_sin(double, double, int);
extern double __kernel_cos(double, double);
extern double __kernel_tan(double, double, int);
extern int __kernel_rem_pio2(double *, double *, int, int, int, const __int32_t *);
extern __int32_t __ieee754_rem_pio2f(float, float *);
extern int __kernel_rem_pio2f(float *, float *, int, int, int, const __int32_t *);
#endif
