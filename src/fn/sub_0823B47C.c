#include "global.h"

/* tentative: drifts an offset along an 8-way heading and sets facing from it */

#define ActorMotion_ApplyHeadingDrift sub_0823B47C

#ifdef NONMATCHING
// fuzzy: 99.4%
// 4 bytes off: ROM places the `adds r2, r1, #0` copy before the facing strb;
// agbcc always emits it after (byte-field store path). ~3000 variants tried.

s32 Math_Atan2(s32, s32);
extern const s16 gUnk_086149C4[];

struct Obj {
    u8 unk0[0x30];
    s16 x, y, z;
    u8 unk36[0x34C - 0x36];
    s16 heading;
    u8 unk34E[0x3A2 - 0x34E];
    u8 frame;
    u8 flip;
    u8 dir;
    u8 unk3A5[0x4A8 - 0x3A5];
    s16 baseX, baseY, baseZ;
    u8 unk4AE[2];
    s16 offX, offY, offZ;
    s16 init;
    u8 unk4B8;
    u8 enabled;
};

void Player_ClampCameraOffsets_0823B3C4(struct Obj *);

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void ActorMotion_ApplyHeadingDrift(struct Obj *p)
{
    if (p->init == 0) {
        p->baseX = p->x;
        p->baseY = p->y + 150;
        p->baseZ = p->z;
        p->offX = 0;
        p->offY = 0;
        p->offZ = 0;
        p->init = 1;
        Player_ClampCameraOffsets_0823B3C4(p);
    }
    if (p->heading != -1) {
        u32 a = ((p->heading + 1) & 7) * 32;
        p->offX -= scale(gUnk_086149C4[(a + 64) & 255] * 30);
        p->offZ -= scale(gUnk_086149C4[a] * 30);
        Player_ClampCameraOffsets_0823B3C4(p);
    }
    if ((p->offX != 0 || p->offZ != 0) && p->enabled) {
        s32 a = ((u16)Math_Atan2(p->offX, p->offZ) + 0x70) >> 5;
        u32 d = 7;
        s8 f;
        d &= a;
        f = p->dir = d;
        if ((u16)f > 4) {
            p->frame = 8 - f;
            p->flip = 1;
        } else {
            p->frame = f;
            p->flip = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", ActorMotion_ApplyHeadingDrift);
#endif
