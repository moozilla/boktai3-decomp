#include "global.h"

/* tentative: picks a directional animation and moves along the facing with a nudge near the cell edge */
#define ArmorActor_UpdateDirectedMotion sub_081826C8

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
extern u32 gUnk_0824EB94[];
extern s16 gUnk_086149C4[];
void Armor_SetAnimationFromStart(u32, u32);
void Sound_PlaySongChecked(u32);

static inline s32 RoundMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

void ArmorActor_UpdateDirectedMotion(u8 *p, s32 speed, u32 timer)
{
    u8 *s = p + 0xB40;
    s16 zero;
    struct ActorMotionVector *v;

    if (*s == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EB94[((p[0xB39] + 0x60) >> 6) & 3]);
        (*s)++;
        p[0xB41] = 1;
        *(u16 *)(p + 0xC4) = timer;
        ((struct ActorAnimationFlags *)(p + 0x238))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x1E4))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x28C))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x190))->flags =
            four | ((struct ActorAnimationFlags *)(p + 0x190))->flags;
        {
            struct ActorMotionFlags *m = (void *)(p + 0x180);
            u32 one = ~1;
            m->flags &= one;
        }
        ((struct ActorStateFlags *)(p + 0xB32))->flags &= ~0x20;
    }
    {
        u8 *ap = p + 0xB39;
        struct ActorMotionVector *w = v = (void *)(p + 0x50);
        u8 a = *ap;
        s32 t;
        w->x = RoundMotion(gUnk_086149C4[(a + 64) & 255] * speed);
        w->y = 0;
        t = RoundMotion(gUnk_086149C4[a] * speed);
        zero = 0;
        w->z = t;
    }
    if (*(s16 *)(p + 0x50) == 0) {
        s32 d = 0x80 - p[0x48];
        if (d > 16)
            *(s16 *)(p + 0x50) = 3;
        else if (d < -16) {
            u32 n = 0xFFFD;
            *(u16 *)(p + 0x50) = n;
        }
    } else if (*(s16 *)(p + 0x54) == 0) {
        s32 d = 0x80 - p[0x4C];
        if (d > 16)
            *(s16 *)(p + 0x54) = 3;
        else if (d < -16) {
            u32 n = 0xFFFD;
            *(u16 *)(p + 0x54) = n;
        }
    }
    if ((*(u16 *)(p + 0xC8) & 3) == 1) {
        *(s16 *)(p + 0x50) >>= 2;
        *(s16 *)(p + 0x54) >>= 2;
    }
    switch (*(u16 *)(p + 0xC8)) {
        case 2:
            if (*(u16 *)(p + 0xC0) == zero)
                Sound_PlaySongChecked(0xC2);
            break;
        case 5:
            if (*(u16 *)(p + 0xC0) == zero)
                Sound_PlaySongChecked(0xC3);
            break;
    }
}
