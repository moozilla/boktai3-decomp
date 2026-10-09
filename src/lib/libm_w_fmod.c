/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 w_fmod.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define fmod sub_0824B3CC
#include "libm_compat.h"
extern const char gUnk_08602E50[];

extern int *__errno (void);
extern const char * const _sys_errlist[];
extern int _sys_nerr;
 double fmod(double x, double y)
{
 double z;
 struct exception exc;
 z = __ieee754_fmod(x,y);
 if(__fdlib_version == __fdlibm_ieee ||isnan(y)||isnan(x)) return z;
 if(y==0.0) {
            exc.type = 1;
            exc.name = (char *)gUnk_08602E50;
     exc.arg1 = x;
     exc.arg2 = y;
     exc.err = 0;
            if (__fdlib_version == __fdlibm_svid)
               exc.retval = x;
     else
        exc.retval = 0.0/0.0;
            if (__fdlib_version == __fdlibm_posix)
               (*__errno()) = 33;
            else if (!matherr(&exc)) {
                  (*__errno()) = 33;
            }
     if (exc.err != 0)
        (*__errno()) = exc.err;
            return exc.retval;
 } else
     return z;
}
