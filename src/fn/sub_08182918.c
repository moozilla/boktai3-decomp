
#ifdef NONMATCHING
// fuzzy: 93.9%
// foundry draft (worker best 93.4%); what differs is mostly register allocation and instruction order
#define Armor_UpdateRunWithMotion sub_08182918
#define gArmorAnimTable2 gUnk_0824EBA4
#define gSineTable gUnk_086149C4
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

extern u32 gArmorAnimTable2[];
extern s16 gSineTable[];

void Armor_SetAnimationFromStart(u32, u32);
void Sound_PlaySongChecked(u32 song);

static inline s32 RoundMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

void sub_08182918(u8 *p, u32 speed, u32 initial)
{
    if (p[0xB40] == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p, (u32)&gArmorAnimTable2[((p[0xB39] + 0x60) >> 6) & 3]);
        p[0xB40]++;
        p[0xB41] = 1;
        *(u16 *)(p + 0xC4) = initial;
        ((struct ActorAnimationFlags *)(p + 0x238))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x1E4))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x28C))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x190))->flags =
            four | ((struct ActorAnimationFlags *)(p + 0x190))->flags;
        {
            struct ActorMotionFlags *motion = (void *)(p + 0x180);
            u32 one = 1;
            motion->flags &= ~one;
        }
        ((struct ActorStateFlags *)(p + 0xB32))->flags |= 0x20;
    }

    {
        u8 *s = p + 0xB39;
        struct ActorMotionVector *v = (void *)(p + 0x50);
        u8 angle = *s;
        struct ActorMotionVector *vx = v;
        v->x = RoundMotion(gSineTable[(angle + 64) & 255] * speed);
        v->y = 0;
        v->z = RoundMotion(gSineTable[angle] * speed);

        if (vx->x == 0) {
            s32 diff = 0x80 - p[0x48];
            if (diff > 0x10)
                vx->x = 3;
            else if (diff < -0x10)
                vx->x = -3;
        } else {
            s16 *z = (s16 *)(p + 0x54);
            if (*z == 0) {
                s32 diff = 0x80 - p[0x4C];
                if (diff > 0x10)
                    *z = 3;
                else if (diff < -0x10)
                    *z = -3;
            }
        }
    }

    switch (*(u16 *)(p + 0xC8)) {
        case 2:
            if (*(u16 *)(p + 0xC0) == 0)
                Sound_PlaySongChecked(0xC2);
            break;
        case 5:
            if (*(u16 *)(p + 0xC0) == 0)
                Sound_PlaySongChecked(0xC3);
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08182918);
#endif
