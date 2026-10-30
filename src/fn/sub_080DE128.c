// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
// confidence=low: chooses response 0x75/0x82/0x87 from target range, heading, and action timers.
#define FIELD(p, n, T) (*(T *)((u8 *)(p) + (n)))
extern const u8 gUnk_0824E2A8[];
void Dog_SelectRetreatOrChase(void *);
static inline u8 Flag(u8 *context, u32 mask)
{
    if (FIELD(context, 0x678, u32) & mask)
        return 1;
    return 0;
}
static inline u8 FarFlag(u8 *context, u32 mask)
{
    u32 bit = mask;
    register u32 offset asm("r3") = 0x678;
    asm("" : "+r"(offset));
    {
        register u32 *field asm("r0") = (u32 *)(context + offset);
        if (*field & bit)
            return 1;
    }
    return 0;
}
static inline u8 OutsideFlag(u8 *context, u32 mask)
{
    u32 bit = mask;
    u32 offset = 0x678;
    {
        u32 *field = (u32 *)(context + offset);
        if (*field & bit)
            return 1;
    }
    return 0;
}
static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 bit = mask;
    asm("" : "+r"(bit));
    return actor->flags2D4 & bit;
}
static inline s32 Absolute(s32 value)
{
    s32 result = value;
    if (result < 0)
        result = -result;
    return result;
}
static inline void Response(struct SceneActorRegistryResetView *actor, u8 *nav, u32 behavior,
                            u32 response)
{
    Chase_SetState(actor, 0, 2, behavior);
    Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 0x10);
    SceneActorReaction_ResetResponseState(nav + 0x8C, gUnk_0824E2A8, response);
}
void Dog_SelectTargetAction(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    u8 *nav = (u8 *)&actor->group.common;
    if (ActorFlag(actor, 0x100))
        return;
    if (actor->tracking.distance < actor->tracking.range) {
        struct ContactPosition delta;
        s32 difference, angle;
        {
            struct ContactPosition *out = &delta;
            struct ContactPosition *from = &actor->body.position;
            struct ContactPosition *to = &((struct ChaseTarget *)actor->tracking.target)->position;
            SceneActorTerrain_Subtract(out, to, from);
        }
        difference = Math_Atan2(delta.x, delta.z) - actor->body.direction;
        if (difference > 128)
            difference -= 256;
        else if (difference < -128)
            difference += 256;
        angle = Absolute(difference);
        if (angle <= 23) {
            u32 mode = FIELD(actor, 0x1E6, u16);
            if (mode == 0 || mode == 2) {
                if (actor->tracking.distance < FIELD(context, 0x67C, u32) && !FIELD(context, 0x698, u16)) {
                    Response(actor, nav, 0, 0x75);
                    return;
                }
            fallback:
                if (Flag(context, 8))
                    return;
                goto move;
            } else {
                if (actor->tracking.distance < FIELD(context, 0x67C, u32) && !FIELD(context, 0x698, u16)) {
                    Response(actor, nav, 0, 0x82);
                    return;
                }
                if (actor->tracking.distance < FIELD(context, 0x680, u32) && angle <= 5 &&
                    !FIELD(context, 0x69A, u16)) {
                    Response(actor, nav, 1, 0x87);
                    return;
                }
                goto fallback;
            }
        }
        if (!FarFlag(context, 8)) {
        move:
            Dog_SelectRetreatOrChase(actor);
        }
    } else {
        if (!OutsideFlag(context, 8))
            Dog_SelectRetreatOrChase(actor);
    }
}
