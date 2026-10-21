#ifndef GUARD_GLOBAL_H
#define GUARD_GLOBAL_H

#include "gba/types.h"
#include "gba/defines.h"

/* Not-yet-decompiled functions inside a C unit: the generated asm for NAME is
 * assembled in place (build/asm/nonmatching/NAME.s). */
#define INCLUDE_ASM(DIR, NAME) asm(".include \"build/asm/nonmatching/" #NAME ".s\"")
asm(".include \"asm/macros.inc\"");

/* Fixed RAM addresses as integer constants, for code that agbcc must see as a
 * literal (a constant address is loaded by reload, which changes register
 * choice; see docs/REGALLOC.md). Each names its symbol in a trailing comment;
 * tools/check_ram_literals.py verifies the address against the link map on
 * every build, so these cannot drift from the symbols. */
/* An empty statement macro: the shape a debug check compiled out of a release
 * build takes. Its empty loop still splits the code around it, which some
 * functions need to match. The original name is unknown (confidence=low). */
#define DEBUG_NOP()                                                                                \
    do {                                                                                           \
    } while (0)
#define RANDOM_TABLE_ADDR 0x0203B400 /* gUnk_0203B400 */

/* libagbsyscall */
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);

#endif
