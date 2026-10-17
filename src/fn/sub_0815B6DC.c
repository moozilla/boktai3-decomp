#include "global.h"

struct P {
    u8 filler[0x3A2];
    u8 b3A2;
    u8 b3A3;
    u8 dir;
    u8 filler2[0x58C - 0x3A5];
    void *fn;
    u8 filler4[0x598 - 0x590];
    u16 h598;
    u8 filler5[0x5A4 - 0x59A];
    u32 c;
};

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_08151078(void);

void sub_0815B6DC(struct P *p, s32 dir, u32 h, u32 c)
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
    p->fn = sub_08151078;
}
