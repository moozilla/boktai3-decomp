#include "global.h"
#include "contact_actor.h"
#ifdef NONMATCHING
// fuzzy: 63.2%
// Loop-invariant layout matches (table, &offset, index, 1023, zero hoisted; sine table kept in
// the loop by setting it twice). Remaining: the ROM keeps the sine pointer in r2 for the second
// lookup instead of reloading it, v[1] reuses the hoisted zero, and i/table registers rotate.
struct SpriteEffectController;
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);

static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}

static inline int step(const u16 *table)
{
    u32 next = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = next;
    return table[next];
}

void sub_0817AEC8(u8 *p, int direction)
{
    int i;
    s16 offset[4];
    for (i = 0; i < 6; i++) {
        const u16 *table = gUnk_0203B400;
        s16 *v = offset;
        int angle = ((step(table) >> 4) & 63);
        int radius = ((step(table) >> 4) & 31);
        int life;
        const s16 *sine;
        angle = angle + direction - 32;
        radius = radius + i * 4 + 2;
        sine = gUnk_086149C4;
        v[0] = Scale(sine[(angle + 64) & 255] * radius);
        v[1] = 0;
        sine = gUnk_086149C4;
        v[2] = Scale(sine[angle & 255] * radius);
        life = ((step(table) >> 4) & 15) + 16;
        SpriteEffectController_Create((struct ContactPosition *)(p + 0x54),
                                      (struct ContactPosition *)offset, 0x1C1E, 0xD43F, 0, 0, 0x11,
                                      1, 1, 2, 0, life, 0, 0, 0, 0, -4, -4);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817AEC8);
#endif
