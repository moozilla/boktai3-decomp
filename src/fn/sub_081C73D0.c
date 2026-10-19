#include "global.h"
#define StateActor_UpdateAnimation13State sub_081C73D0
#define B(o) (*(u8 *)((u8 *)p + (o)))
#define W(o) (*(u32 *)((u8 *)p + (o)))
struct S {
    u8 f[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 g[0xd4 - 0xa];
    u8 d4[1];
};
struct Vec {
    s16 x, y, z, pad;
};
extern const s16 gUnk_086149C4[];
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
void StateActor_UpdateAnimation13State(u32 a, struct S *p)
{
    u32 r;
    u32 speed;

    if (p->b9 != 0) {
        p->b9 = 0;
        p->b7 = 0;
        switch (p->b1) {
            case 4: case 5: case 6: case 7: case 8: case 9: case 10:
            case 11: case 12: case 13: case 14: case 15: case 16:
            case 17: case 18: case 19: case 20: case 21: case 22:
            case 23: case 24: case 25: case 26:
                r = 0;
                break;
            case 27:
                r = 13;
                break;
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 1, 4);
    }

    speed = W(0x208);
    if (speed == 0) {
        p->b7 = 1;
        return;
    }
    {
        struct Vec *vel = (struct Vec *)((u8 *)p + 0x70);
        u8 angle = B(0x1b);
        u32 spd = speed;
        vel->x = shift12(spd * gUnk_086149C4[(angle + 0x40) & 0xff]);
        vel->y = 0;
        vel->z = shift12(spd * gUnk_086149C4[angle]);
    }
    W(0x208) = W(0x208) * 3 / 4;
}
