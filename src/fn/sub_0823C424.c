#include "global.h"

s32 Div(s32, s32);

static inline u32 FullState(u8 *p)
{
    if (!p)
        return 0;
    if (*(u16 *)(p + 0x538) != 0) {
        if (*(u16 *)(p + 0x534) == 1)
            return 1;
    }
    return 0;
}
static inline u32 FullFlag(u8 *p)
{
    u32 *field;
    u32 bits;
    if (!p)
        return 0;
    field = (u32 *)(p + 0x448);
    bits = 0x40;
    if (*field & bits)
        return 1;
    return 0;
}
#ifdef NONMATCHING
// fuzzy: 93.0%
// Remaining differences: boolean materialization belongs to the flag predicate rather than the state predicate.

// confidence=low: flags and active modifiers choose and shorten the linked action delay.
#define Player_ComputeLinkedActionDelay sub_0823C424
s32 Player_ComputeLinkedActionDelay(u8 *p)
{
    s32 n, t;
    if (FullState(p) || FullFlag(p))
        goto full;
    n = Div(0, 100);
    if (n > 16) {
    full:
        n = 16;
    } else if (n <= 8) {
        n = 9;
    }
    if (p[0x5B9] == 2 && p[0x5BB] != 0) {
        n -= 4;
    }
    if (p[0x4F6] != 0) {
        n -= p[0x4F6];
    }
    if (n <= 1) {
        return 2;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823C424);
#endif
