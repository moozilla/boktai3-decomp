#include "global.h"

struct P0819716C {
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

void sub_0819716C(struct P0819716C *t)
{
    switch (t->state) {
    case 0:
        sub_08193680(t, 0);
        t->x = 0;
        t->pad = 0;
        t->y = -0x680;
        sub_08195014(t, 0x260, 0x500);
        t->state++;
        Sound_PlaySongChecked(0x402);
        break;
    case 1: {
        s32 x, z;
        sub_08195140(t);
        t->a += 0xc00;
        if (t->a > 0)
            t->a = 0;
        t->c += 0x2000;
        if (t->c > 0)
            t->c = 0;
        x = t->a + t->b;
        z = t->c;
        t->x = (x >> 8) + 0x260;
        t->y = (z >> 8) + 0x500;
        if (t->a >= 0 && t->c >= 0) {
            t->state++;
            Trigger_ConsumePending(t->queue, 1);
        }
        break;
    }
    }
}
