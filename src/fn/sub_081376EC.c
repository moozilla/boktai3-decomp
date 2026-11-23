#include "scene_actor_control_begin.h"
#include "terrain_sampling.h"
#include "scene_actor_terrain_reaction.h"
#include "scene_actor_behavior.h"
#include "scene_actor_chase_shared.h"
#include "global.h"
#include "scene_actor_terrain.h"
#include "scene_actor_stimulus.h"

s32 Callback_InvokeActor(void *, void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorAttachment_RetargetNearestPlayer(void *, struct ContactPosition *);
s8 SceneStimulus_FindOverlapFromIndex(s32, s32, u32, u8, u16, s32);
u32 sub_081376B4(void *, struct ContactPosition *, s32);
u8 SceneActorTarget_CanReplaceByDistance(void *, struct ContactPosition *, u32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))



static inline void SetWordFlags(u32 *flags, u32 input)
{
    u32 mask = input;
    *flags |= mask;
}





static inline u32 CellHeight(u32 cell, struct ContactPosition *position)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}

struct RoutePoint {
    s16 x, z;
};
struct RouteLink {
    u8 unknown0[8];
    struct RoutePoint *point;
};

#define GROUP_FIELD(g, off, T) (*(T *)((u8 *)(g) + (off)))

static inline u8 MoveTo(struct ChaseActor *actor, u8 *group, struct ContactPosition *position,
                        u16 *flags, u32 blocked)
{
    void *shape = GROUP_FIELD(group, 0x1fc, void *);
    struct ContactPosition *destination = &GROUP_FIELD(group, 0x58, struct ContactPosition);
    s32 result;
    if (!Nav_IsPositionInMesh(shape, position, destination)) {
        *flags &= ~blocked;
        return 0;
    }
    result = Terrain_ApplyBlocked(shape, position, destination);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->base.body.motion, 8);
        return 1;
    }
    *flags &= ~blocked;
    return 0;
}

// confidence=low: scene actor picks the closest reachable overlapping stimulus (8 slots), arms its control timer and walks there.
#define SceneActorTarget_ApproachNearestStimulus sub_081376EC
u32 SceneActorTarget_ApproachNearestStimulus(struct ChaseActor *actor)
{
    u8 *group;
    struct SceneActorRegistryControl *control;
    struct ContactPosition candidate;
    struct ContactPosition delta;
    u32 found;
    u32 bestTag;
    s32 best;
    s8 i;
    u16 *flagsp;
    u16 *timer;
    struct ContactPosition *dest;
    struct ContactPosition *target;
    u32 bit;
    struct ContactPosition *position;
    s32 settle = 360;

    {
        u32 busy = SceneActorStimulus_CheckBusy(actor);
        if (busy)
            return 0;
        group = (u8 *)&actor->group.common;
        actor->group.common.control.mode = busy;
    }
    found = 0;
    best = 0x7FFFFFFF;
    bestTag = 0;
    i = 0;
    position = &actor->base.body.position;
    for (; i <= 7; i++) {
        u32 tag;
        s32 next;
        next = SceneStimulus_FindOverlapFromIndex(
            actor->base.body.position.x, actor->base.body.position.z, GROUP_FIELD(group, 0x4E, u16),
            GROUP_FIELD(group, 0x57, u8), actor->base.body.id, (s8)i);
        i = next;
        if (next < 0)
            break;
        tag = sub_081376B4(actor, &candidate, (s8)i);
        if (SceneActorTarget_CanReplaceByDistance(actor, &candidate, tag) &&
            Nav_IsPositionInMesh(GROUP_FIELD(group, 0x1fc, void *), &actor->base.body.position,
                                 &candidate)) {
            s32 distance;
            SceneActorTerrain_Subtract(&delta, &candidate, &actor->base.body.position);
            distance = delta.x * delta.x + delta.z * delta.z;
            if (best > distance) {
                best = distance;
                GROUP_FIELD(group, 0x58, struct ContactPosition) = candidate;
                bestTag = tag;
                found = 1;
            }
        }
    }
    if (!found)
        return 0;
    timer = &GROUP_FIELD(group, 0x54, u16);
    if (*timer == 0) {
        control = (struct SceneActorRegistryControl *)(group + 0x4C);
        SceneActorChase_AndWordFlags(&actor->base.behaviorFlags, 0xFFFEBE3F);
        control->counter = 0;
        FIELD(control, 0x14, u16) = 0;
        control->timer = 0;
        *timer = 180;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
        SceneActorControl_Begin(actor, GROUP_FIELD(group, 0x68, void *), 9, 0);
        SceneActorTerrain_EnterState(actor, 0, 0, 11);
    }
    {
        u32 mask = 0x80;
        u16 *p = &actor->base.motionFlags;
        u32 t = *p & mask;
        flagsp = p;
        if (t) {
            u16 *value = &FIELD(actor, 0x418, u16);
            *value += 300;
            if (*value > FIELD(actor, 0x416, u16))
                *value = FIELD(actor, 0x416, u16);
        }
    }
    GROUP_FIELD(group, 0x64, u32) = bestTag;
    GROUP_FIELD(group, 0x60, u16) = settle;
    dest = GROUP_FIELD(group, 0x200, struct ContactPosition *);
    {
        struct RouteLink *link = (void *)(group + 0x74);
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(Terrain_GetCellIndex(dest), dest);
    }
    bit = 0x200;
    GROUP_FIELD(group, bit, struct ContactPosition *)->y = actor->base.body.position.y;
    GROUP_FIELD(group, bit, struct ContactPosition *)->pad = 0;
    if (!MoveTo(actor, group, position, flagsp, bit)) {
        Callback_InvokeActor(actor, GROUP_FIELD(group, 0x224, void *));
    } else {
        {
            u32 mask = 0x10;
            actor->base.behaviorFlags |= mask;
        }
        SceneActorChase_OrHalfFlags(flagsp, 0x200);
        actor->base.field2C2 = 0x20;
        {
            u32 mask = 0x1000000;
            actor->base.statusFlags |= mask;
        }
        SceneActorAttachment_RetargetNearestPlayer(
            actor, &GROUP_FIELD(group, 0x58, struct ContactPosition));
        if (GROUP_FIELD(group, 0x6C, void *))
            Callback_InvokeActor(actor, GROUP_FIELD(group, 0x6C, void *));
    }
    return 1;
}
