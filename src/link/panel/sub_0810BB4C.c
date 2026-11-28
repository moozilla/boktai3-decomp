#include "global.h"
struct LinkResults {
    u8 pad0[0x154];
    u16 positive[2];
    u16 negative[2];
};

static inline u8 Valid(s32 i)
{
    if (i < 0)
        return 0;
    if (i > 1)
        return 0;
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
        p->positive[i]++;
        p->positive[i] = LinkResults_LimitCounter(p->positive[i], 0, 20);
    }
}

// Tentative: increments one side's negative result counter, then caps it at 20.
void sub_0810BB90(struct LinkResults *p, s32 i)
{
    if (p && Valid(i)) {
        p->negative[i]++;
        p->negative[i] = LinkResults_LimitCounter(p->negative[i], 0, 20);
    }
}
