#include "global.h"

struct P08196BE4 {
    u8 f0[0x18];
    u8 queue[0x5404 - 0x18];
    s16 x;
    s16 pad;
    s16 y;
    u8 f1[0x724a - 0x540a];
    u8 state;
    u8 f2[0x72a8 - 0x724b];
    s32 a, b, c, d, e, f;
};
void sub_08193680(void *, u32);
void sub_08195014(void *, s32, s32);
void sub_08195140(void *);
void Sound_PlaySongChecked(u32);
void Trigger_ConsumePending(void *, u32);

void sub_08196BE4(struct P08196BE4 *t)
{
    switch (t->state) {
    case 0:
        sub_08193680(t, 0);
        t->x = 0;
        t->pad = 0;
        t->y = -0x680;
        sub_08195014(t, 0x200, 0x980);
        t->state++;
        Sound_PlaySongChecked(0x400);
        break;
    case 1: {
        s32 z;
        sub_08195140(t);
        z = t->c += 0x2000;
        t->x = ((t->a + t->b) >> 8) + 0x200;
        t->y = (z >> 8) + 0x980;
        if (t->c >= 0) {
            t->state++;
            t->d = -0xc00;
            t->e = 0x88;
            t->f = 0x400;
        }
        break;
    }
    case 2: {
        s32 z;
        sub_08195140(t);
        t->a += t->d;
        z = t->c += 0x2000;
        t->x = ((t->a + t->b) >> 8) + 0x200;
        t->y = (z >> 8) + 0x980;
        if (t->a < -0x60000) {
            t->state++;
            Trigger_ConsumePending(t->queue, 1);
        }
        break;
    }
    }
}
