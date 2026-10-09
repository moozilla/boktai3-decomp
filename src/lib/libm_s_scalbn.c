/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_scalbn.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define scalbn sub_0824D8B8
#include "libm_compat.h"

#define two54 (1.80143985094819840000e+16)
#define twom54 (5.55111512312578270212e-17)
#define huge (1.0e+300)
#define tiny (1.0e-300)

 double scalbn (double x, int n)
{
 __int32_t k,hx,lx;
 do { ieee_double_shape_type ew_u; ew_u.value = (x); (hx) = ew_u.parts.msw; (lx) = ew_u.parts.lsw; } while (0);
        k = (hx&0x7ff00000)>>20;
        if (k==0) {
            if ((lx|(hx&0x7fffffff))==0) return x;
     x *= two54;
     do { ieee_double_shape_type gh_u; gh_u.value = (x); (hx) = gh_u.parts.msw; } while (0);
     k = ((hx&0x7ff00000)>>20) - 54;
            if (n< -50000) return tiny*x;
     }
        if (k==0x7ff) return x+x;
        k = k+n;
        if (k > 0x7fe) return huge*copysign(huge,x);
        if (k > 0)
     {do { ieee_double_shape_type sh_u; sh_u.value = (x); sh_u.parts.msw = ((hx&0x800fffff)|(k<<20)); (x) = sh_u.value; } while (0); return x;}
        if (k <= -54)
            if (n > 50000)
  return huge*copysign(huge,x);
     else return tiny*copysign(tiny,x);
        k += 54;
 do { ieee_double_shape_type sh_u; sh_u.value = (x); sh_u.parts.msw = ((hx&0x800fffff)|(k<<20)); (x) = sh_u.value; } while (0);
        return x*twom54;
}
