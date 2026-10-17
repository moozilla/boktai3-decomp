#include "global.h"

struct P {
    u8 filler[0x418];
    u8 state;
    u8 filler3[0x58C - 0x419];
    void *fn;
    u16 target[3];
    u8 filler4[0x5A4 - 0x596];
    u32 c;
};

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void sub_081561EC(void);

u32 sub_0815C498(struct P *p, u16 *pos, u32 c)
{
    u32 r;

    p->c = c;
    if (p->state == 2) {
        sub_0815AD78(p);
        sub_0813AFEC(p, 0x26, 0);
        p->fn = sub_081561EC;
        p->target[0] = pos[0];
        p->target[1] = pos[1];
        p->target[2] = pos[2];
        r = 1;
    } else {
        r = 0;
    }
    return r;
}
