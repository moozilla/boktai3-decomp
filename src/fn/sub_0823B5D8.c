#include "global.h"
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

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

// confidence=low: initializes a fixed radial offset opposite the stored action heading.
#define Player_InitFacingDriftOffset sub_0823B5D8
void Player_InitFacingDriftOffset(struct Obj *p)
{
    if (!p->init) {
        u32 angle;
        p->baseX = p->x;
        p->baseY = p->y + 150;
        p->baseZ = p->z;
        angle = ((*(u8 *)((u8 *)p + 0x4A5) + 1) & 7) << 5;
        p->offX = -scale(gUnk_086149C4[(angle + 64) & 255] * 800);
        p->offY = 0;
        p->offZ = -scale(gUnk_086149C4[angle] * 800);
        p->init = 1;
    }
}
