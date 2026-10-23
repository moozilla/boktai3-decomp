#include "global.h"

#ifdef NONMATCHING
// fuzzy: 94.3%
// fix.py best version (reprinted C)
// confidence=low: projects a cosine-scaled speed opposite the actor side.
#define Hresvelgr_SetCosineScaledVelocity sub_0819C8F8
struct BurstVector {
    s16 x, y, z, pad;
};
struct BurstActor {
    u8 unknown0[0x51];
    u8 direction;
    u8 unknown52[2];
    struct BurstVector position;
    struct BurstVector velocity;
    u8 unknown64[0x110 - 0x64];
    struct BurstVector *target;
    u8 unknown114[0x32B - 0x114];
    u8 side;
    u8 unknown32C[0x39C - 0x32C];
    struct BurstVector travelTarget;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void MotionEntry_CreateWithDelta(void *, void *, s32, s32);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct BurstVector *v, s16 angle, s8 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[((angle + 64) << 24) >> 24] * radius);
}

static inline void add(struct BurstVector *d, struct BurstVector *a, struct BurstVector *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}
// Remaining differences: inlined vector-helper register and constant lifetimes.
void Hresvelgr_SetCosineScaledVelocity(struct BurstActor *actor, s32 radius, s32 angle)
{
    s32 direction = ((actor->side << 6) + 128) & 255;
    s32 speed = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    typeof(&actor->velocity) t1;
    t1 = &actor->velocity;
    polar(t1, direction, speed);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0819C8F8);
#endif
