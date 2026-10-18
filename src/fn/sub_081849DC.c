#define gActorRetreatAnimations gUnk_0824EED0
#define gSineTable              gUnk_086149C4
// Names describe observed behavior; the action interpretation is tentative.
#include "global.h"

struct ActorAnimationFlags {
    u16 unk0, unk2, unk4;
    u32 flags : 16;
};
struct ActorStateFlags {
    u32 flags : 16;
};
struct ActorMotionFlags {
    u32 unk0, flags;
};
struct ActorMotionVector {
    s16 x, y, z;
};
extern u32 gUnk_0824EED0[];
extern s16 gUnk_086149C4[];
u32 ActorController_SetAnimation(u32, u32, u32);

static inline s32 RoundMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

static inline void SetMotion(struct ActorMotionVector *v, s32 angle, s32 speed)
{
    v->x = RoundMotion(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = RoundMotion(gUnk_086149C4[angle] * speed);
}

void ActorController_UpdateRetreatMotion(u8 *p)
{
    u8 *s = p + 0xB40;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four = 4;
            ActorController_SetAnimation((u32)p, (u32)&gUnk_0824EED0[((p[0xB39] + 0x60) >> 6) & 3],
                                         2);
            (*s)++;
            p[0xB41] = st;
            *(u16 *)(p + 0xC4) = 12;
            ((struct ActorAnimationFlags *)(p + 0x238))->flags &= ~four;
            ((struct ActorAnimationFlags *)(p + 0x1E4))->flags &= ~four;
            ((struct ActorAnimationFlags *)(p + 0x28C))->flags &= ~four;
            ((struct ActorAnimationFlags *)(p + 0x190))->flags =
                four | ((struct ActorAnimationFlags *)(p + 0x190))->flags;
            {
                u32 one = 1;
                ((struct ActorMotionFlags *)(p + 0x180))->flags &= ~one;
            }
            ((struct ActorStateFlags *)(p + 0xB32))->flags |= 0x20;
        }
        case 1: {
            SetMotion((void *)(p + 0x50), (p[0xB39] + 0x80) & 255, 20);
            if (p[0xB43]) {
                p[0xB40] = 0xFF;
                p[0xB41] = 1;
            }
            break;
        }
    }
}
