#define gActorTimedMotionAnimations gUnk_0824EEE0
#define gSineTable                  gUnk_086149C4
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
extern u32 gUnk_0824EEE0[];
extern s16 gUnk_086149C4[];
void Armor_SetAnimationFromStart(u32, u32);

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

void ActorController_UpdateTimedMotion(u8 *p)
{
    u8 *s = p + 0xB40;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EEE0[((p[0xB39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xB41] = st;
            ((struct ActorAnimationFlags *)(p + 0x238))->flags &= ~four;
            ((struct ActorAnimationFlags *)(p + 0x1E4))->flags &= ~four;
            ((struct ActorAnimationFlags *)(p + 0x28C))->flags &= ~four;
            ((struct ActorAnimationFlags *)(p + 0x190))->flags =
                four | ((struct ActorAnimationFlags *)(p + 0x190))->flags;
            {
                struct ActorMotionFlags *motion = (void *)(p + 0x180);
                u32 one = 1;
                motion->flags |= one;
            }
            ((struct ActorStateFlags *)(p + 0xB32))->flags |= 0x20;
        } break;
        case 1:
            if (p[0xB42] > 9) {
                *s = 0xFF;
                p[0xB41] = st;
            }
            break;
    }
    SetMotion((void *)(p + 0x50), p[0xB3B], 25);
}
