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
// angle is s16 and the z product reads radius * table: both shape the
// allocation. The s16 conversion adds RTL that combine folds away later, but
// flow counts it, so speed outlives actor and is allocated after it; the
// operand order puts speed's REG_DEAD note first, so it does not inherit the
// table's r3 preference (tools/scope/README.md, facts 11 and 16).
static inline void polar(struct BurstVector *v, s16 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(radius * gUnk_086149C4[angle & 255]);
}

void Hresvelgr_SetSineScaledVelocity(struct BurstActor *actor, s32 radius, s32 angle)
{
    u32 direction = ((actor->side << 6) + 128) & 255;
    polar(&actor->velocity, direction, scale(gUnk_086149C4[angle & 255] * radius));
}
