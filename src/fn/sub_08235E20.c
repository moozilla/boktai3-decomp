#include "global.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 93.8%
// Open: only register choice differs; the ROM keeps each scaled delta in the
// register that held the speed factor (r4/r1), mine picks r1/r2.
struct SweepSprite2 {
    u32 flags;
    u8 pad4[0x14];
    u16 x;
    u16 y;
    u16 z;
    u8 pad1E[0xA];
    u8 active;
    u8 frame;
    u8 direction;
    u8 turn;
    u16 timer;
    u16 rise;
    u16 speed;
    u8 pad32[6];
    u16 baseX;
    u8 pad3A[2];
    u16 baseZ;
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
static inline s32 scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

void sub_08235E20(struct SweepSprite2 *p, void *anim)
{
    if (++p->timer > 11) {
        p->flags |= 1;
        p->active = 0;
        return;
    }
    if (p->timer == 6)
        SpriteDrawable_SelectTileFrame(p, anim, p->frame + 1);
    if (p->timer > 3) {
        p->rise++;
        if (p->timer > 8)
            p->turn--;
    }
    p->direction += p->turn;
    {
        u32 force = p->speed;
        s32 delta = scale(gUnk_086149C4[(p->direction + 64) & 255] * force);
        p->x = p->baseX + delta;
    }
    {
        u32 force = p->speed;
        s32 delta = scale(gUnk_086149C4[p->direction] * force);
        p->z = p->baseZ + delta;
    }
    p->y += p->rise;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08235E20);
#endif
