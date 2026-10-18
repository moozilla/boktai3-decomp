#include "global.h"

struct P08196D54 {
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
void Nidhoggr_SetBackgroundMode(void *, u32);
void Nidhoggr_InitRelativeMotion(void *, s32, s32);
void Sound_PlaySongChecked(u32);
void Trigger_ConsumePending(void *, u32);

void Nidhoggr_UpdateRightwardPass(struct P08196D54 *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 1);
            t->x = -0xd80;
            t->pad = 0;
            t->y = 0xb80;
            Nidhoggr_InitRelativeMotion(t, -0x680, 0xb80);
            t->state++;
            Sound_PlaySongChecked(0x405);
            break;
        case 1: {
            s32 x, z;
            t->a += 0x3000;
            x = t->a + t->b;
            z = t->c;
            t->x = (x >> 8) - 0x680;
            t->y = (z >> 8) + 0xb80;
            if (t->a >= 0)
                t->state++;
            break;
        }
        case 2: {
            s32 x, z;
            t->a += 0x3000;
            x = t->a + t->b;
            z = t->c;
            t->x = (x >> 8) - 0x680;
            t->y = (z >> 8) + 0xb80;
            if (t->a > 0x2ffff) {
                t->state++;
                Trigger_ConsumePending(t->queue, 1);
            }
            break;
        }
    }
}
