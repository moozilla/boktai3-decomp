#include "global.h"
#include "axe.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

// confidence=low: after the wait, selects a route point and starts the stored movement spline.

void Axe_SelectRandomRoutePoint(struct S080FA7D0 *s);
void Axe_SetMovementEndpoints(struct S080FAA20 *s, struct P *d);
u8 sub_080FAA5C(struct ChaseActor *actor);

struct AxeVec {
    s16 x, y, z;
};
struct AxeRoutePoint {
    s16 x, z;
};
struct AxeGroup {
    u8 unknown0[0x44];
    u16 field44;
    u8 unknown46[0x36];
    struct AxeRoutePoint *point;
};
struct AxeLink {
    u8 unknown0[8];
    struct AxeRoutePoint *point;
};
static inline u32 CellIndex_UpdateRouteDestinationPhase(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 CellHeight(struct AxeVec *position, u32 cell)
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
static inline void SetState_UpdateRouteDestinationPhase(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior, u32 counter)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = counter;
    actor->base.field2B1 = 1;
}

void Axe_UpdateRouteDestinationPhase(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    u32 zero;
    struct AxeVec targetStorage;
    struct AxeVec *target;
    struct AxeVec *copy;
    struct AxeRoutePoint *point;
    struct AxeLink *link;
    s16 deltaX;
    s16 deltaZ;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if (((s32)actor->base.counter2AC) > 60) {
        Axe_SelectRandomRoutePoint(actor);
        target = &targetStorage;
        link = (struct AxeLink *)(((u8 *)actor) + 0x448);
        point = link->point;
        target->x = point->x;
        target->z = point->z;
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            copy = target;
            target->y = CellHeight(target, CellIndex_UpdateRouteDestinationPhase(cellX, cellZ));
        }
        zero = 0;
        Axe_SetMovementEndpoints(actor, copy);
        sub_080FAA5C(actor);
        SetState_UpdateRouteDestinationPhase(actor, 0, 0, 1, zero);
        {
            struct AxeVec *position = (struct AxeVec *)(&actor->base.body.position);
            target = &targetStorage;
            target->x -= position->x;
            copy->y -= position->y;
            copy->z -= position->z;
        }
        deltaZ = copy->z;
        deltaX = target->x;
        *(((u8 *)actor) + 0x3cd) = Math_Atan2(deltaX, deltaZ);
        ZeroFill8(context->clearF14);
    } else
        actor->base.counter2AC++;
}
