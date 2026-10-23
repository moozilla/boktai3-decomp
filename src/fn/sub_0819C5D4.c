#include "global.h"

#define Hresvelgr_GetTravelTarget sub_0819C5D4
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
extern u32 gUnk_03005308;
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
static inline s32 randomValue(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
#ifdef NONMATCHING
// Remaining differences: redundant angle mask, table load and argument-copy order.
void Hresvelgr_GetTravelTarget(struct BurstActor *actor, struct BurstVector *out)
{
    if (out) {
        s32 radius = (randomValue() >> 3) % 512 + 256;
        s32 sample = randomValue();
        s32 angle = ((actor->side << 6) + ((sample >> 3) % 32 + 112)) & 255;
        polar(out, angle, radius);
        add(out, actor->target, out);
        out->y = 832;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Hresvelgr_GetTravelTarget);
#endif
