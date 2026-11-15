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
 * functions need to match. Like a bare empty do/while(0), it counts as a
 * barrier (docs/WORKER.md). The original name is unknown (confidence=low). */
#define DEBUG_NOP()                                                                                \
    do {                                                                                           \
    } while (0)
#define RANDOM_TABLE_ADDR 0x0203B400 /* gRandomTable */

/* libagbsyscall; the constants as in gba/syscall.h */
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);
#define CPU_SET_SRC_FIXED 0x01000000
#define CPU_SET_32BIT 0x04000000
#include "gba/macro.h"

/* Zero the 8 bytes at dest with the SDK's CpuFill32. The game's own wrapper:
 * as a function its argument is evaluated before the SDK macro stores the zero,
 * which sets the stack order and registers of 128 call sites (a bare CpuFill32
 * stores first). The original name is unknown (confidence=low). */
static inline void ZeroFill8(void *dest)
{
    CpuFill32(0, dest, 8);
}

#endif
