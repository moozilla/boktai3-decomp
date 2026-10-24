#include "global.h"
#include "contact_actor.h"
// MATCHING_CONCESSION: register-pinning
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
    const u16 *table;
    int i = 0;
    s16 offset[4];
    {
        const u16 *randomTable = gUnk_0203B400;
        asm("" : "+r"(randomTable));
        table = randomTable;
    }
    for (; i < 6; i++) {
        s16 *v = offset;
        int angle = ((step(table) >> 4) & 63);
        int radius = ((step(table) >> 4) & 31);
        int life;
        int spread;
        int speed;
        int zero = 0;
        int heading;
        register const s16 *sine asm("r2");
        spread = i * 4;
        heading = angle + direction;
        heading -= 32;
        speed = radius + spread;
        speed += 2;
        sine = gUnk_086149C4;
        v[0] = Scale(sine[(heading + 64) & 255] * speed);
        v[1] = zero;
        v[2] = Scale(sine[heading & 255] * speed);
        life = ((step(table) >> 4) & 15) + 16;
        SpriteEffectController_Create((struct ContactPosition *)(p + 0x54),
                                      (struct ContactPosition *)offset, 0x1C1E, 0xD43F, 0, 0, 0x11,
                                      1, 1, 2, 0, life, 0, 0, 0, 0, -4, -4);
    }
}
