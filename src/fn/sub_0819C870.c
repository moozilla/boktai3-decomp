#include "global.h"

// confidence=low: projects a sine-scaled speed opposite the actor side.
#define Hresvelgr_SetSineScaledVelocity sub_0819C870
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
static inline s32 sineScale(s32 angle, s32 radius)
{
    return scale(gUnk_086149C4[angle & 255] * radius);
}
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: inlined vector-helper register and constant lifetimes.
inline static void polar_0819C870(struct BurstVector *v, s32 angle, s8 radius)
{
    v->x = (u16)scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[(u16)(angle & 255)] * radius);
}

void Hresvelgr_SetSineScaledVelocity(struct BurstActor *actor, s32 radius, s32 angle)
{
    s32 direction = ((actor->side << 6) + 128) & 255;
    s32 speed = sineScale(angle, radius);
    polar_0819C870(&actor->velocity, direction, speed);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0819C870);
#endif
