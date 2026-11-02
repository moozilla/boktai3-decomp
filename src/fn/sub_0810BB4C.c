#include "global.h"
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

static inline u16 LinkResults_LimitCounter(u16 value, s32 min, s32 max)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;
    return value;
}
// Tentative: increments one side's positive result counter, then caps it at 20.
void sub_0810BB4C(struct LinkResults *p, s32 i)
{
    if (p && Valid(i)) {
        s32 offset = i * sizeof(u16);
        u16 *counter = p->positive;
        s32 value;
        counter = (u16 *)((u8 *)counter + offset);
        (*counter)++;
        value = LinkResults_LimitCounter(*counter, 0, 20);
        p->positive[i] = value;
    }
}

// Tentative: increments one side's negative result counter, then caps it at 20.
void sub_0810BB90(struct LinkResults *p, s32 i)
{
    if (p && Valid(i)) {
        s32 offset = i * sizeof(u16);
        u16 *counter = p->negative;
        s32 value;
        counter = (u16 *)((u8 *)counter + offset);
        (*counter)++;
        value = LinkResults_LimitCounter(*counter, 0, 20);
        p->negative[i] = value;
    }
}
