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
static inline s32 randomValue(u32 table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(u16 *)(offset + table);
}
// One variable holds the table address and later the angle (the ROM keeps both in r4): set twice,
// it is not a constant register, so its priority is not halved and it wins r4 over the 1023 mask
// (update_equiv_regs in local-alloc.c only marks a pseudo whose every set stores one constant).
void Hresvelgr_SetRandomTravelTarget(struct BurstActor *actor)
{
    struct BurstActor *saved = actor;
    u32 work = (u32)gUnk_0203B400;
    s32 radius = (randomValue(work) >> 3) % 512 + 256;
    s32 sample = randomValue(work);
    s32 base = actor->side << 6;
    s32 offset = (sample >> 3) % 32 + 112;
    work = base + offset;
    work &= 255;
    polar(&actor->travelTarget, work, radius);
    {
        struct BurstVector *target = saved->target;
        add(&actor->travelTarget, target, &actor->travelTarget);
    }
    actor->travelTarget.y = 832;
}
