#include "global.h"

/* tentative: strafe motion along 0xB3B with backward animation mode */
#define Armor_UpdateStrafeMotion sub_08182B14

/* tentative: picks a directional animation and moves along the facing with a nudge near the cell edge */

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
extern u32 gUnk_0824EBB4[];
extern s16 gUnk_086149C4[];
u32 ActorController_SetAnimation(u32, u32, u32);
void Sound_PlaySongChecked(u32);

static inline s32 RoundMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

void Armor_UpdateStrafeMotion(u8 *p, s32 speed, u32 timer)
{
    u8 *s = p + 0xB40;
    struct ActorMotionVector *v;

    if (*s == 0) {
        u32 four = 4;
        u8 a = p[0xB39];
        u32 dir = ((a + 0x60) >> 6) & 3;
        u32 mode;
        s32 d = p[0xB3B] - a;
        mode = 0;
        if ((u32)((d & 0xFF) - 0x41) <= 0x7E)
            mode = 2;
        ActorController_SetAnimation((u32)p, (u32)&gUnk_0824EBB4[dir], mode);
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
        ((struct ActorStateFlags *)(p + 0xB32))->flags |= 0x20;
    }
    {
        u8 *ap = p + 0xB3B;
        struct ActorMotionVector *w = v = (void *)(p + 0x50);
        u8 a = *ap;
        s32 t;
        w->x = RoundMotion(gUnk_086149C4[(a + 64) & 255] * speed);
        w->y = 0;
        t = RoundMotion(gUnk_086149C4[a] * speed);
        w->z = t;
    }
    if ((*(u16 *)(p + 0xC8) & 3) == 1) {
        *(s16 *)(p + 0x50) >>= 2;
        *(s16 *)(p + 0x54) >>= 2;
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
