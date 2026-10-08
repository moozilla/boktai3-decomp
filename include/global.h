#ifndef GUARD_GLOBAL_H
#define GUARD_GLOBAL_H

#include "gba/types.h"
#include "gba/defines.h"

/* Not-yet-decompiled functions inside a C unit: the generated asm for NAME is
 * assembled in place (build/asm/nonmatching/NAME.s). */
#define INCLUDE_ASM(DIR, NAME) asm(".include \"build/asm/nonmatching/" #NAME ".s\"")
asm(".include \"asm/macros.inc\"");

/* libagbsyscall */
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);

#endif
