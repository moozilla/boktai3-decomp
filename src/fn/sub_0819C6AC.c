#include "global.h"

// confidence=low: stores a randomized elevated travel point in the actor.
#define Hresvelgr_SetRandomTravelTarget sub_0819C6AC
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
static inline s32 randomValue(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: inlined vector-helper register and constant lifetimes.
inline static s32 randomValue_0819C6AC(void)
{
    u32 offset;
    {
        u16 k1 = 1023;
        gUnk_03005308 = (gUnk_03005308 + 1) & k1;
    }
    offset = gUnk_03005308 * 2;
    return *((const u16 *)(offset + ((u32)gUnk_0203B400)));
}

void Hresvelgr_SetRandomTravelTarget(struct BurstActor *actor)
{
    struct BurstActor *saved = actor;
    s32 radius = ((randomValue_0819C6AC() >> 3) % 512) + 256;
    s32 sample = randomValue_0819C6AC();
    s32 angle = ((actor->side << 6) + (((sample >> 3) % 32) + 112)) & 255;
    typeof(saved->target) t1;
    polar(&actor->travelTarget, angle, radius);
    t1 = saved->target;
    add(&actor->travelTarget, t1, &actor->travelTarget);
    actor->travelTarget.y = 832;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0819C6AC);
#endif
