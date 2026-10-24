#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best (sweep after the inlined-insn fix)
// Open: register allocation only. The original keeps p in r4, i*2 in r2 and
// the clamped count in r3; this draft swaps p and i*2 (p in r3, i*2 in r4).
// Pinning the count to r3 reaches ~82% but p/i*2 stay swapped; pinning p
// breaks the address CSE.

struct LinkResults {
    u8 pad0[0x154];
    u16 positive[2];
    u16 negative[2];
};

static inline u8 Valid(s32 i)
{
    if (i < 0)
        goto no;
    if (i <= 1)
        goto yes;
no:
    return 0;
yes:
    return 1;
}

static inline void SetPositive(struct LinkResults *p, s32 i, u16 v)
{
    p->positive[i] = v;
}

static inline void SetNegative(struct LinkResults *p, s32 i, u16 v)
{
    p->negative[i] = v;
}

// confidence=low: bumps one side's positive result counter, saturating at 20.
void sub_0810BB4C(struct LinkResults *p, s32 i)
{
    if ((p != ((void *)0)) && Valid(i)) {
        u16 *c = &p->positive[i];
        u8 v;
        do {
            (*c)++;
        } while (0);
        v = (v > 20) ? (20) : (*c);
        SetPositive(p, i, v);
    }
}

// confidence=low: bumps one side's negative result counter, saturating at 20.
void sub_0810BB90(struct LinkResults *p, s32 i)
{
    if ((p != ((void *)0)) && Valid(i)) {
        u16 *c = &p->negative[i];
        u8 v;
        do {
            (*c)++;
        } while (0);
        v = (v > 20) ? (20) : (*c);
        SetNegative(p, i, v);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0810BB4C);
INCLUDE_ASM("asm/nonmatching", sub_0810BB90);
#endif
