// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "scene_actor_chase.h"
/* Tentative: the context pending flag requests a delayed restart from the route origin. */
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
void SceneActor_SetPositionXZ(void *, s32, s32);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (actor->flags2D0 & mask)
            return 1;
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
struct GhoulDelayPacket {
    u8 unknown0[8];
    u8 modes[3];
    u8 unknownB;
    u32 delay;
    void *control;
};
struct GhoulRouteOrigin {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};
struct GhoulGroup {
    u8 unknown0[0x48];
    void *delayControl;
    u8 unknown4C[0x28];
    struct SceneActorRegistryRoute route;
    u8 unknown88[4];
    struct GhoulDelayPacket delay;
    u8 unknownA0[0x15C];
    struct GhoulRouteOrigin *routeOrigin;
};
struct GhoulContext {
    u8 unknown0[0x628];
    u16 pending;
    u8 unknown62A[2];
    struct ContactPosition target;
    u8 unknown634[0x14];
    u16 timer;
};
static inline void SaveDelay(struct GhoulDelayPacket *packet, void *control, u32 behavior,
                             u32 delay)
{
    packet->delay = delay;
    packet->control = control;
    packet->modes[0] = 0;
    packet->modes[1] = 0;
    packet->modes[2] = behavior;
}
static inline u8 TestPending(struct GhoulContext *context, u32 mask, u16 **pending)
{
    u16 *pointer = &context->pending;
    u32 value = *pointer & mask;
    *pending = pointer;
    if (value)
        return 1;
    return 0;
}
s32 Ghoul_ResumePendingRoute(struct SceneActorRegistryResetView *actor)
{
    register struct GhoulContext *context = actor->context;
    struct GhoulGroup *group = (void *)&actor->group;
    struct GhoulRouteOrigin **routeSlot;
    u16 *flags;
    u16 *pending;
    u16 *timer;
    struct GhoulRouteOrigin *route;
    void *body;
    if (Unavailable(actor))
        return 1;
    if (!TestPending(context, 1, &pending))
        return 1;
    {
        u32 mask = 0xA107;
        flags = &actor->flags2D4;
        if (mask & (*flags))
            return 1;
    }
    timer = &context->timer;
    if (*timer)
        (*timer)--;
    else {
        {
            u32 *p = &actor->flags2D0;
            u32 mask = ~4;
            *p = mask & (*p);
        }
        body = &actor->body.id;
        routeSlot = &group->routeOrigin;
        route = *routeSlot;
        SceneActor_SetPositionXZ(body, route->x, route->z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SaveDelay(&group->delay, group->delayControl, 12, 30);
        Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        Chase_SetState(actor, 3, 1, 0);
        SceneBody_StartRoute(body, &group->route, group->route.parameterA, group->route.parameterB,
                             group->route.parameterC);
        {
            void *route = *routeSlot;
            register struct ContactPosition *position asm("r4") = &actor->body.position;
            struct ContactPosition *target = &context->target;
            s32 result = Nav_IsPositionInMesh(route, position, target);
            if (!result) {
                u32 mask = ~0x200;
                *flags = mask & (*flags);
            } else {
                result = Terrain_ApplyBlocked(route, position, target);
                if (result) {
                    if (result != 2)
                        Memory_Clear(&actor->body.motion, 8);
                } else {
                    u32 mask = ~0x200;
                    *flags = mask & (*flags);
                }
            }
        }
        {
            u32 mask = ~1;
            *pending = mask & (*pending);
        }
    }
    return 1;
}
