#include "global.h"

struct P {
    u8 filler[0x3A2];
    u8 b3A2;
    u8 b3A3;
    u8 dir;
    u8 filler2[0x418 - 0x3A5];
    u8 state;
    u8 filler3[0x58C - 0x419];
    void *fn;
    u8 filler4[0x598 - 0x590];
    u16 h598;
    u8 filler5[0x5A4 - 0x59A];
    u32 c;
};

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_08152A5C(void);
void sub_08154AF0(void);
void sub_08150C5C(void);

void sub_0815B52C(struct P *p, s32 dir, u32 h, u32 c)
{
    if (dir >= 0)
        p->dir = dir;
    p->dir |= 1;
    if (p->dir > 4) {
        p->b3A2 = (8 - p->dir) >> 1;
        p->b3A3 = 1;
    } else {
        p->b3A2 = p->dir >> 1;
        p->b3A3 = 0;
    }
    p->h598 = h;
    p->c = c;
    sub_0815AD78(p);
    sub_0813AFEC(p, 0x1D, 0);
    if (p->state == 3)
        p->fn = sub_08152A5C;
    else if (p->state == 4)
        p->fn = sub_08154AF0;
    else
        p->fn = sub_08150C5C;
}
