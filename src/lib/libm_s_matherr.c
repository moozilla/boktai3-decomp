/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this software is freely
 * granted, provided that this notice is preserved.
 */
/* Adapted from newlib 1.8.2 s_matherr.c; see docs/LIBM.md. */
// COMPILER: old_agbcc
// CFLAGS: -O2 -fno-builtin
#define matherr sub_0824D8A4
#include "libm_compat.h"

 int matherr(struct exception *x)
{
 int n=0;
 if(x->arg1!=x->arg1) return 0;
 return n;
}
