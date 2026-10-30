#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
#include "ram.h"
u8 SceneActorPath_FaceAndCheckStraightPath(void *);
s32 Math_Atan2(s32, s32);
struct SceneActorTrackingTarget {
    u8 unknown0[12];
    struct ContactPosition position;
};
struct SceneActorTrackingParent {
    u8 unknown0[0x6F2];
    u16 flags;
};
static inline u8 IsPlayerTracked(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 2;
    if (actor->flags2C0 & mask)
        return 1;
    return 0;
}
static inline void SubtractPosition(struct ContactPosition *out, struct ContactPosition *a,
                                    struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
s32 SceneActor_TryTrackNearbyPlayer(struct SceneActorRegistryResetView *actor,
                                    struct ContactPosition *input, u32 playerId)
{
    u8 *group = (void *)&actor->group;
    struct ContactPosition delta;
    struct ContactPosition *deltaPointer;
    struct ContactPosition *position;
    u32 distance;
    struct SceneActorReactionMotion *motion;
    if (playerId != gTrackedPlayerId[0])
        goto fail;
    {
        struct Player *player = gPlayers[0];
        u32 mask = 8;
        if (!(player->statusFlags & mask))
            goto fail;
    }
    deltaPointer = &delta;
    SubtractPosition(deltaPointer, input, &actor->body.position);
    distance = delta.x * delta.x + deltaPointer->z * deltaPointer->z;
    {
        u32 range = *(u32 *)(group + 0x50);
        position = &actor->body.position;
        if (distance >= range)
            goto fail;
    }
    if (!(u8)SceneActorPath_FaceAndCheckStraightPath(actor))
        goto fail;
    {
        u32 *flags = &actor->flags2CC;
        s32 mask = -0x81;
        *flags &= mask;
    }
    {
        struct SceneActorTrackingParent *parent = actor->context;
        u32 mask = 0x800;
        u16 *flags = &parent->flags;
        u32 value = *flags;
        mask |= value;
        *flags = mask;
    }
    motion = (void *)&actor->tracking.target;
    motion->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
    if (IsPlayerTracked(actor))
        motion->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
    else if (!motion->fieldDC)
        goto ready;
    {
        struct SceneActorTrackingTarget *target = (void *)motion->fieldDC;
        s32 dx = target->position.x - position->x;
        s32 dz = target->position.z - actor->body.position.z;
        actor->tracking.heading = Math_Atan2(dx, dz);
        actor->tracking.distance = dx * dx + dz * dz;
    }
ready: {
    u32 mask = 0x80;
    u16 *flags = &actor->flags2D6;
    u32 value = *flags;
    mask |= value;
    *flags = mask;
}
    return 1;
fail:
    return 0;
}
