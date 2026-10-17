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
    u8 filler4[0x5A4 - 0x590];
    u32 c;
};

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_0813ADD0(struct P *);
void sub_08152174(void);
void sub_081543F0(void);
void sub_08155008(void);
void sub_081503EC(void);

static inline u8 sideDir(u8 d)
{
    if ((u8)(d - 2) <= 4)
        return 4;
    return 0;
}

void sub_0815B858(struct P *p, s32 dir, u32 c)
{
    if (dir >= 0)
        p->dir = dir;
    p->c = c;
    sub_0815AD78(p);
    sub_0813AFEC(p, 7, 0);
    if (p->state == 3) {
        p->dir |= 1;
        if (p->dir > 4) {
            p->b3A2 = (8 - p->dir) >> 1;
            p->b3A3 = 1;
        } else {
            p->b3A2 = p->dir >> 1;
            p->b3A3 = 0;
        }
        p->fn = sub_08152174;
    } else {
        u8 *d = &p->dir;
        *d = sideDir(*d);
        sub_0813ADD0(p);
        if (p->state == 4) {
            p->fn = sub_081543F0;
        } else if (p->state == 5) {
            p->fn = sub_08155008;
        } else {
            p->b3A2 >>= 2;
            p->b3A3 = 0;
            p->fn = sub_081503EC;
        }
    }
}
