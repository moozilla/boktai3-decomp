#include "global.h"
// confidence=low: approaches the scene anchor, then restores retarget control on expiry.
#define Ivy_UpdateAnchorApproach sub_080F2538
struct IvyAnchorScene {
    u8 unknown0[0x30];
    u16 x, y, z;
};
struct IvyAnchorActor {
    u8 unknown0[0x60];
    struct IvyAnchorScene *scene;
    u16 motionX, unknown66, motionZ;
    u8 unknown6A[0xA];
    s16 driftX, unknown76, driftZ;
    u8 unknown7A[0x13];
    u8 flags8D, unknown8E[4];
    u8 state, phase, unknown94[0x3F4];
    u16 targetX, targetY, targetZ, unknown48E;
    s16 deltaX, deltaY, deltaZ;
    u8 unknown496[0x1B], direction;
    u8 unknown4B2[0x6D2];
    u16 x, y, z;
    u8 unknownB8A[0x13E];
    void (*memberUpdate)(void);
    u8 unknownCCC[8];
    void (*control)(void);
    u8 unknownCD8[0x1C];
    u32 timer;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void ChildGroup_UpdateOrbitingMembers(void);
void Ivy_WaitForRetarget(void);
static inline s32 Ivy_ScaleDirection(s32 value)
{
    if (value >= 0)
        return value >> 10;
    return -((-value) >> 10);
}
static inline s32 Ivy_Halve(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}
static inline void Ivy_SetMode(struct IvyAnchorActor *actor, u8 state, u8 phase)
{
    actor->state = state;
    actor->phase = phase;
    state |= actor->flags8D;
    actor->flags8D = state;
}
void Ivy_UpdateAnchorApproach(struct IvyAnchorActor *actor)
{
    s32 x, z;
    actor->targetX = actor->scene->x;
    actor->targetY = actor->scene->y;
    actor->targetZ = actor->scene->z;
    actor->deltaX = actor->targetX - actor->x;
    actor->deltaY = actor->targetY - actor->y;
    actor->deltaZ = actor->targetZ - actor->z;
    actor->direction = Math_Atan2(actor->deltaX, actor->deltaZ);
    x = gUnk_086149C4[(actor->direction + 64) & 255];
    z = gUnk_086149C4[actor->direction];
    actor->motionX = Ivy_ScaleDirection(x);
    actor->motionZ = Ivy_ScaleDirection(z);
    actor->x += actor->motionX + actor->driftX;
    actor->z += actor->motionZ + actor->driftX;
    actor->driftX = Ivy_Halve(actor->driftX);
    actor->driftZ = Ivy_Halve(actor->driftZ);
    if (--actor->timer == 0) {
        actor->timer = 300;
        Ivy_SetMode(actor, 1, 3);
        actor->memberUpdate = ChildGroup_UpdateOrbitingMembers;
        actor->control = Ivy_WaitForRetarget;
    }
}
