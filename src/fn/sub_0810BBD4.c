// MATCHING_CONCESSION: register-pinning
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
    i = 0;
    {
        register struct LinkScores *first asm("r2") = p;
        s32 count;
        asm("" : "+r"(first));
        count = first->count;
        if (i < count)
            do {
                s32 bonus, score;
                register s32 positive asm("r12");
                s32 negative;
                if (times[i] < times[1 - i])
                    bonus = Div(times[1 - i] - times[i], 120) + 40;
                else {
                    bonus = -10;
                    bonus -= Div(times[i] - times[1 - i], 120);
                }
                {
                    struct LinkResults *check = results;
                    u32 result;
                    if (!check)
                        result = 0;
                    else if (!Valid(i)) {
                        {
                            register u32 zero asm("r1") = 0;
                            asm("" : "+r"(zero));
                            positive = zero;
                        }
                        goto negative;
                    } else
                        result = results->positive[i];
                    positive = result;
                }
            negative: {
                register struct LinkResults *check asm("r2") = results;
                asm("" : "+r"(check));
                if (!check || !Valid(i))
                    negative = 0;
                else
                    negative = results->negative[i];
            }
                p->points[i] = (positive * 7 - (negative * 13 - 50)) + bonus;
                score = p->points[i];
                if (score > 100)
                    score = 100;
                else if (score < 0)
                    score = 0;
                p->points[i] = score;
                {
                    s32 total = p->totals[i] + p->points[i];
                    if (total > 9999)
                        total = 9999;
                    else if (total < 0)
                        total = 0;
                    p->totals[i] = total;
                }
            } while (++i < p->count);
    }
    if (i <= 3) {
        register u32 initialMask asm("r2") = 0xffff;
        u32 absentMask;
        asm("" : "+r"(initialMask));
        absentMask = initialMask;
        asm("" : "+r"(absentMask));
        {
            register u32 offset asm("r0") = i * 2 + 0x84;
            register u8 *base asm("r4");
            register u16 *initialEntry asm("r3");
            u16 *entry;
            base = (u8 *)p;
            asm("" : "+r"(base));
            initialEntry = (u16 *)(offset + (u32)base);
            asm("" : "+r"(initialEntry));
            entry = initialEntry;
            do {
                MarkAbsent(entry, absentMask);
                MarkAbsent(entry + 4, absentMask);
                entry++;
            } while (++i <= 3);
        }
    }
}
