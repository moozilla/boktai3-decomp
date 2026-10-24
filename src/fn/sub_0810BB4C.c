#include "global.h"
// MATCHING_CONCESSION: register-pinning
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

static inline void LinkResults_SetPositiveByOffset(struct LinkResults *p, s32 byteOffset, u16 value)
{
    u16 *base = p->positive;
    *(u16 *)((u8 *)base + byteOffset) = value;
}

static inline void SetNegative(struct LinkResults *p, s32 i, u16 v)
{
    p->negative[i] = v;
}

// Tentative: increments one side's positive result counter, then caps it at 20.
void sub_0810BB4C(struct LinkResults *p, s32 i)
{
    if (p != NULL && Valid(i)) {
        register s32 byteOffset asm("r2") = i * sizeof(u16);
        u16 *counter = p->positive;
        s32 value;
        counter = (u16 *)((u8 *)counter + byteOffset);
        (*counter)++;
        value = *counter;
        if (value > 20)
            value = 20;
        LinkResults_SetPositiveByOffset(p, byteOffset, value);
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
