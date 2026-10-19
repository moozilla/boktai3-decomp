#include "global.h"

/* tentative: fixed-speed armor motion along facing with cell nudge and burst */

/* tentative: faces a direction, sets a fixed-speed motion vector, nudges toward the cell centre and spawns a particle burst */

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
extern u32 gUnk_0824EDF8[];
extern u8 gUnk_0824EE08[];
extern s16 gUnk_086149C4[];
void Armor_SetAnimationFromStart(u32, u32);
void SceneParticleBurst_CreateWithPause(void *, s32, void *, s32, s32, s32);

static inline s32 RoundMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

static inline void NudgeAxis(s16 *axis, u8 *frac)
{
    s32 d = *frac;
    d = 0x80 - d;
    if (d > 16)
        *axis = 3;
    else if (d < -16) {
        u32 n = 0xFFFD;
        *(u16 *)axis = n;
    }
}

void ArmorActor_UpdateFixedSpeedMotion(u8 *p)
{
    u8 *s = p + 0xB40;
    s16 zero;
    struct ActorMotionVector *v;
    struct ActorMotionVector *w;

    if (*s == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EDF8[((p[0xB39] + 0x60) >> 6) & 3]);
        *s = 0xFF;
        p[0xB41] = 1;
        ((struct ActorStateFlags *)(p + 0xB32))->flags |= 2;
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
        *(u16 *)(s - 14) &= ~0x20;
    }
    if ((p[0xB42] & 0x1F) == 0) {
        u8 *ap = p + 0xB39;
        u8 a;
        s32 t;
        s32 m = 32; /* a variable keeps the second product a muls */
        w = v = (void *)(p + 0x50);
        a = *ap;
        w->x = RoundMotion(gUnk_086149C4[(a + 64) & 255] * 32);
        w->y = 0;
        t = RoundMotion(gUnk_086149C4[a] * m);
        zero =
            a >> 8; /* always 0 (u8 >> 8); combine folds it late, leaving the dead `movs r7, #0` */
        w->z = t;
        *(u16 *)(p + 0xB32) |= 0x20;
        if (*(s16 *)(p + 0x50) == zero)
            NudgeAxis((s16 *)(p + 0x50), p + 0x48);
        else if (*(s16 *)(p + 0x54) == 0)
            NudgeAxis((s16 *)(p + 0x54), p + 0x4C);
        SceneParticleBurst_CreateWithPause(p + 0x48, 0, gUnk_0824EE08, 8, 3, zero);
    } else {
        ((struct ActorStateFlags *)(p + 0xB32))->flags &= ~0x20;
    }
}
