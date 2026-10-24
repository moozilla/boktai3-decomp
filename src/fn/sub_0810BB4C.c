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

static inline void SetNegative(struct LinkResults *p, s32 offset, u16 v)
{
    u16 *c = p->negative;
    c = (u16 *)((u8 *)c + offset);
    *c = v;
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
    if ((p != NULL) && Valid(i)) {
        s32 offset = i * 2;
        u16 *c = p->negative;
        register s32 v asm("r3");
        asm("" : "+r"(offset));
        c = (u16 *)((u8 *)c + offset);
        (*c)++;
        v = *c;
        if (v > 20)
            v = 20;
        SetNegative(p, offset, v);
    }
}
