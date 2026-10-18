#include "global.h"

struct P0819627C {
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
void Nidhoggr_UpdateAnchoredSway(void *, s32, s32);
void Nidhoggr_SetBehavior(void *, u32);

void Nidhoggr_UpdateFastReturn(struct P0819627C *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            Nidhoggr_InitRelativeMotion(t, 0x1e0, 0x500);
            if (t->a < 0)
                t->d = 0x800;
            else if (t->a > 0)
                t->d = 0x90;
            else
                t->d = 0;
            t->state = 1;
            Sound_PlaySongChecked(0x40b);
        case 1:
            t->a += t->d;
            if (t->a > 0 && t->d > 0) {
                t->a = 0;
                t->d = 0;
            } else if (t->a < 0 && t->d < 0) {
                t->a = 0;
                t->d = 0;
            }
            Nidhoggr_UpdateAnchoredSway(t, 0x1e0, 0x500);
            if (t->a == 0 && t->x == 0x1e0) {
                t->x = 0x1e0;
                t->b = 0;
                t->e = 0;
                if (t->x == 0x1e0 && t->y > 0x4fb && t->y <= 0x504) {
                    t->y = 0x500;
                    Nidhoggr_SetBehavior(t, 6);
                }
            }
            break;
    }
}
