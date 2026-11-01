#include "global.h"
#include "scene_actor_reaction.h"
#include "player.h"
#include "engine.h"

void *SceneActorContacts_FindObjectById(u16);
u8 SceneActor_FindNearbyActivePlayer(struct SceneActorRegistryResetView *);
u8 SceneActor_IsCollisionTargetUnavailable(struct SceneActorRegistryResetView *);
static inline u8 motionFlag(struct SceneActorRegistryResetView *actor)
{
    u8 *motion = (u8 *)&actor->tracking.target;
    u32 mask = 2;
    if (motion[15] & mask)
        return 1;
    return 0;
}
static inline u8 nearFlag(struct SceneActorRegistryResetView *actor)
{
    u8 *motion = (u8 *)&actor->tracking.target;
    u32 mask = 8;
    if (motion[15] & mask)
        return 1;
    return 0;
}
static inline u8 collisionMode(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
static inline void setTargetFlags(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 0x80;
    u16 *flags = &actor->flags2D6;
    u8 *motion;
    mask |= *flags;
    *flags = mask;
    motion = (u8 *)&actor->tracking.target;
    {
        u32 mask = 0x10;
        mask |= motion[15];
        motion[15] = mask;
    }
}
s32 SceneActor_AcquireCollisionTarget(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    struct SceneActorChaseTarget *target;
    s32 d;
    if (!motionFlag(actor))
        goto zero;
    if ((s32)actor->field304 > 0) {
        setTargetFlags(actor);
        actor->tracking.target = (u32)SceneActorContacts_FindObjectById((u16)actor->field304);
        return 1;
    }
    if (collisionMode()) {
        if (SceneActor_FindNearbyActivePlayer(actor))
            goto yes;
        if (!SceneActor_IsCollisionTargetUnavailable(actor))
            goto collision;
        goto zero;
    } else {
        d = actor->body.position.y - gPlayers[0]->position.y;
        if (d < 0)
            d = -d;
        if (d > 0x200)
            goto zero;
        if (!SceneActor_FindNearbyActivePlayer(actor))
            goto collision;
    }
yes:
    setTargetFlags(actor);
    return 1;
collision:
    target = actor->tracking.target;
    if (!target)
        return 0;
    if (actor->tracking.distance >= actor->tracking.limit)
        goto zero;
    if (!Nav_IsPositionInMesh(*(void **)(group + 0x1FC), &actor->body.position, &target->position))
        goto zero;
    if (!nearFlag(actor))
        goto zero;
    setTargetFlags(actor);
zero:
    return 0;
}
