#include "global.h"
struct LinkScores {
    u8 pad0[0x1a];
    u16 count;
    u8 pad1c[0x68];
    s16 points[4];
    s16 totals[4];
    u8 pad94[0x4c];
};
struct LinkResults {
    u8 pad0[0x154];
    u16 positive[2];
    u16 negative[2];
    u8 pad15c[0x10];
    s32 elapsed[2];
};
s32 Div(s32, s32);
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
static inline void MarkAbsent(u16 *p, u32 mask)
{
    u32 value = *p;
    value |= mask;
    *p = value;
}
static inline s32 Time(struct LinkResults *p, s32 i)
{
    if (!p)
        return 0;
    return p->elapsed[i];
}
static inline u16 Positive(struct LinkResults *p, s32 i)
{
    if (!p)
        return 0;
    if (!Valid(i))
        return 0;
    return p->positive[i];
}
static inline u16 Negative(struct LinkResults *p, s32 i)
{
    if (!p)
        return 0;
    if (!Valid(i))
        return 0;
    return p->negative[i];
}
// confidence=low: the formula combines two result counters and relative elapsed time into bounded points.
void LinkSelection_UpdateParticipantScores(struct LinkScores *p)
{
    struct LinkResults *results = (void *)((u8 *)p + 0xe0);
    s32 times[2];
    s32 i;
    times[0] = Time(results, 0);
    times[1] = Time(results, 1);
    for (i = 0; i < p->count; i++) {
        s32 bonus, score, total;
        s32 positive, negative;
        if (times[i] < times[1 - i])
            bonus = Div(times[1 - i] - times[i], 120) + 40;
        else
            bonus = -10 - Div(times[i] - times[1 - i], 120);
        positive = Positive(results, i);
        negative = Negative(results, i);
        p->points[i] = positive * 7 - (negative * 13 - 50) + bonus;
        score = p->points[i];
        if (score > 100)
            score = 100;
        else if (score < 0)
            score = 0;
        p->points[i] = score;
        total = p->totals[i] + p->points[i];
        if (total > 9999)
            total = 9999;
        else if (total < 0)
            total = 0;
        p->totals[i] = total;
    }
    for (; i <= 3; i++) {
        MarkAbsent((u16 *)&p->points[i], 0xFFFF);
        MarkAbsent((u16 *)&p->totals[i], 0xFFFF);
    }
}
