#include "global.h"
#include "engine.h"

struct BurstVector {
    s16 x, y, z, pad;
};
struct BurstActor {
    u8 unknown0[0x51];
    u8 direction;
    u8 unknown52[2];
    struct BurstVector position;
    u8 unknown5C[0x110 - 0x5C];
    struct BurstVector *target;
    u8 unknown114[0x32B - 0x114];
    u8 side;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
void MotionEntry_CreateWithDelta(void *, void *, s32, s32);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct BurstVector *v, s32 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void add(struct BurstVector *d, struct BurstVector *a, struct BurstVector *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}
static inline s32 randomValue(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
// MATCHING_CONCESSION: register-pinning
void Hresvelgr_GetTravelTarget(struct BurstActor *object, struct BurstVector *out)
{
    struct BurstActor *actor = object;
    if (out) {
        register const u16 *table asm("r4") = gUnk_0203B400;
        s32 radius = (randomValue(table) >> 3) % 512 + 256;
        s32 sample = randomValue(table);
        s32 base = actor->side << 6;
        s32 offset = (sample >> 3) % 32 + 112;
        s32 angle = base + offset;
        angle &= 255;
        polar(out, angle, radius);
        add(out, actor->target, out);
        out->y = 832;
    }
}
