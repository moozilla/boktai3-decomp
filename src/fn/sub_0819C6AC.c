#include "global.h"

// MATCHING_CONCESSION: register-pinning

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
static inline s32 randomValue(const u16 *table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(u16 *)(offset + (u32)table);
}
// The table pin is the one concession (as in Hresvelgr_EmitRandomMotionBurst):
// unpinned, the table loses r4 to the 1023 mask constant (priority 576 vs
// 789; AGBCC_PRI=26:+214 matches). See tools/scope/README.md.
void Hresvelgr_SetRandomTravelTarget(struct BurstActor *actor)
{
    struct BurstActor *saved = actor;
    register const u16 *table asm("r4") = gUnk_0203B400;
    s32 radius = (randomValue(table) >> 3) % 512 + 256;
    s32 sample = randomValue(table);
    s32 base = actor->side << 6;
    s32 offset = (sample >> 3) % 32 + 112;
    s32 angle = base + offset;
    angle &= 255;
    polar(&actor->travelTarget, angle, radius);
    {
        struct BurstVector *target = saved->target;
        add(&actor->travelTarget, target, &actor->travelTarget);
    }
    actor->travelTarget.y = 832;
}
