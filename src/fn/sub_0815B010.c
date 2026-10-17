#include "global.h"

struct P {
    u8 filler[0x3A2];
    u8 b3A2;
    u8 b3A3;
    u8 filler2[0x418 - 0x3A4];
    u8 state;
    u8 filler3[0x58C - 0x419];
    void *fn;
    u8 filler4[0x5A4 - 0x590];
    u32 c;
};

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_0814F90C(void);

static inline u8 inRange(u32 q)
{
    if ((u8)(q - 1) <= 1)
        return TRUE;
    return FALSE;
}

static inline u8 above(u32 q)
{
    if (q > 1)
        return TRUE;
    return FALSE;
}

void sub_0815B010(struct P *p, s32 dir, u32 c)
{
    if (p->state == 6) {
        if (p->b3A2 > 1)
            p->b3A2--;
    } else {
        u8 a = (dir << 5) + 0xA0;
        u32 q = ((a + 0x60) >> 6) & 3;
        u8 *a2 = &p->b3A2;
        u8 *a3 = &p->b3A3;
        *a2 = inRange(q);
        *a3 = above(q);
    }
    p->c = c;
    sub_0815AD78(p);
    sub_0813AFEC(p, 0, 3);
    p->fn = sub_0814F90C;
}
