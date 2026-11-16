#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_straight_route.h"
#include "scene_actor_attachment.h"
u8 SceneActorAttachment_TestPositionWindow(struct ChaseActor *, const struct ContactPosition *, u32,
                                           u32);
/* The local player's position, as gPlayers[0] holds it. */
struct MovementModePlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
};
static inline void SetState(struct ChaseActor *actor, u8 s, u8 p, u8 b)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline void SetControl(struct ChaseActor *actor, void *cb, u8 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = cb;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
void SceneActorRegistry_SetMovementMode(struct ChaseActor *actor, s32 mode)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u16 *motionFlags;
    {
        u32 *flags = &actor->base.statusFlags;
        u32 mask = ~4;
        *flags &= mask;
    }
    motionFlags = &actor->base.motionFlags;
    {
        u32 mask = ~0x4000;
        *motionFlags &= mask;
    }
    switch (mode) {
        case 0: {
            struct ContactPosition target;
            u32 *savedFlags;
            {
                u32 mask = 0x10;
                u32 *flags = &actor->base.behaviorFlags;
                *flags |= mask;
            }
            {
                struct ContactPosition *p = &target;
                struct SceneActorChaseRoutePoint *route =
                    ((struct SceneActorRegistryRoute *)((u8 *)actor + 0x448))->point;
                p->x = route->x;
                p->z = route->z;
                {
                    s32 x = p->x >> 8;
                    s32 z = p->z >> 8;

                    savedFlags = &actor->base.behaviorFlags;
                    p->y = SceneActorTerrain_GetHeight(Chase_CellIndex(x, z), p);
                }
            }
            target.y = actor->base.body.position.y;
            SceneActorTerrain_RequestPath(actor, group->route, &actor->base.body.position, &target);
            group->destination->pad = 1;
            if (!SceneActorAttachment_TestPositionWindow(actor, &target, 32, 32)) {
                {
                    u32 *flags = savedFlags;
                    u32 mask = ~0xc0;
                    *flags &= mask;
                }
                SetState(actor, 0, 0, 7);
                SetControl(actor, actor->base.onAlternateEntry, 1);
            } else {
                {
                    u32 *flags = savedFlags;
                    u32 mask = ~0xd0;
                    *flags &= mask;
                }
                SetState(actor, 0, 0, 0);
                SetControl(actor, actor->base.onStanding, 0);
            }
            actor->base.field2C2 = 64;
            break;
        }
        case 1: {
            struct MovementModePlayer **players;
            void *route;
            struct ContactPosition *position;
            struct ContactPosition *target;
            s32 result;
            {
                u16 *flags = (u16 *)&group->common.gate.flags;
                u32 mask = 4;
                mask |= *flags;
                *flags = mask;
            }
            {
                u32 mask = 0x80;
                mask |= *motionFlags;
                *motionFlags = mask;
            }
            group->common.gate.counter = group->common.gate.duration;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            players = (struct MovementModePlayer **)gPlayers;
            route = group->route;
            position = &actor->base.body.position;
            target = &players[0]->position;
            if (Nav_IsPositionInMesh(route, position, target) &&
                (result = Terrain_ApplyBlocked(route, position, target))) {
                if (result != 2)
                    Memory_Clear(&actor->base.body.motion, 8);
            } else {
                u32 mask = ~0x200;
                mask &= *motionFlags;
                *motionFlags = mask;
            }
            SetState(actor, 0, 0, 12);
            SetControl(actor, group->common.gate.control, 26);
            break;
        }
    }
}
