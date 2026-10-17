#include "global.h"

struct Pos {
    u32 x, y;
};

struct P {
    u8 filler[0x418];
    u8 state;
    u8 filler2[0x464 - 0x419];
    u8 b464;
    u8 b465;
    u8 filler3[0x58C - 0x466];
    void *fn;
    struct Pos target;
    u8 filler4[0x59D - 0x598];
    u8 b59D;
    u8 filler5[0x5A4 - 0x59E];
    u32 c;
};

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_08151EB8(void);
void sub_08153A00(void);
void sub_0814FC80(void);

void sub_0815B0A8(struct P *p, struct Pos *pos, u32 c, s32 a)
{
    p->target = *pos;
    p->c = c;
    p->b59D = a;
    if (p->state == 3) {
        if ((s8)a >= 0)
            p->b59D = ((s8)a >> 1) * 2 + 1;
        p->fn = sub_08151EB8;
    } else if (p->state == 4) {
        p->fn = sub_08153A00;
    } else if (p->state == 5) {
        p->b464 = 1;
        p->b465 = 1;
        p->fn = sub_0814FC80;
    } else {
        p->fn = sub_0814FC80;
    }
    sub_0815AD78(p);
    sub_0813AFEC(p, 1, 0);
}
