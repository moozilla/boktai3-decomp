#include "global.h"
#include "engine.h"
#include "ram.h"

// confidence=low: emits three debris particles during the Slime death reaction.

struct Position {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad[0x1E0];
    u16 a;
    u8 pad2[4];
    u16 b;
};
extern const s16 gUnk_086149C4[];
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline u16 Rand(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)table + index);
}
static inline void Polar(s32 angle, struct Position *v, s32 radius, u32 mask)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & mask] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
// confidence=low: death burst of three debris particles.
#define BURST(v)                                                                                   \
    {                                                                                              \
        u32 mask = 255;                                                                            \
        s32 angle;                                                                                 \
        angle = Rand() & 255;                                                                      \
        Polar(angle, &v, (Rand() & 15) + 6, mask);                                                 \
    }
void Slime_EmitDeathDebris(struct Actor *actor)
{
    /* The actor, then (from the first emission on) the actor's position at 0x5C. */
    u8 *base = (u8 *)actor;
    struct Position v;
    u16 *a;
    u16 *b;
    BURST(v)
    v.y = 0x30;
    {
        u32 offset = 0x1E6;
        a = &((struct Actor *)base)->a;
        b = (u16 *)(base + offset);
    }
    {
        u32 sum = *a + *b;
        base += 0x5C;
        sub_08127590(base, &v, 0xB96C, 0xA929, 6, 0x18, 0x200, 0, sum, 0, 0, 0, 0, 0, 0);
    }
    BURST(v)
    v.y = 0x25;
    {
        u32 sum = *a + *b;
        sub_08127590(base, &v, 0xB96C, 0xA929, 7, 0x18, 0x200, 0, sum, 0, 0, 0, 0, 0, 0);
    }
    BURST(v)
    v.y = 0x20;
    {
        u32 sum = *a + *b;
        sub_08127590(base, &v, 0xB96C, 0xA929, 7, 0x18, 0x200, 0, sum, 1, 0, 0, 0, 0, 0);
    }
}
