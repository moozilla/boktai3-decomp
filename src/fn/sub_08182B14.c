#include "global.h"

#ifdef NONMATCHING
// fuzzy: 94.4%
// foundry draft (worker best 93.8%); what differs is mostly register allocation and instruction order
#define Armor_UpdateStrafeWithMotion sub_08182B14
#define gArmorAnimTable3             gUnk_0824EBB4
#define gSineTable                   gUnk_086149C4

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

extern u32 gArmorAnimTable3[];
extern s16 gSineTable[];

u32 ActorController_SetAnimation(u32, u32, u32);
void Sound_PlaySongChecked(u32 song);

static inline s32 RoundMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

void sub_08182B14(u8 *p, u32 speed, u32 initial)
{
    if (p[0xB40] == 0) {
        {
            u8 byte1 = p[0xB39];
            u32 index = ((byte1 + 0x60) >> 6) & 3;
            u32 raw = p[0xB3B] - byte1;
            u32 flag = 0;
            u32 diff = raw & 0xFF;
            if (diff - 0x41 <= 0x7e)
                flag = 2;
            ActorController_SetAnimation((u32)p, (u32)&gArmorAnimTable3[index], flag);
        }
        p[0xB40]++;
        p[0xB41] = 1;
        *(u16 *)(p + 0xC4) = initial;
        {
            u32 four = 4;
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
    }

    {
        u8 *s = p + 0xB3B;
        struct ActorMotionVector *v = (void *)(p + 0x50);
        u8 angle = *s;
        v->x = RoundMotion(gSineTable[(angle + 64) & 255] * speed);
        v->y = 0;
        v->z = RoundMotion(gSineTable[angle] * speed);

        if ((*(u16 *)(p + 0xC8) & 3) == 1) {
            u16 *xp = (u16 *)v;
            *xp = (s16)*xp >> 2;
            {
                u16 *zp = (u16 *)(p + 0x54);
                *zp = (s16)*zp >> 2;
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
INCLUDE_ASM("asm/nonmatching", sub_08182B14);
#endif
