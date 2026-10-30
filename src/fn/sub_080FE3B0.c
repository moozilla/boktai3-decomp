#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

// confidence=low: after the wait, selects a route point and starts the stored movement spline.

void Axe_SelectRandomRoutePoint(void *);
void Axe_SetMovementEndpoints(void *, void *);
void sub_080FAA5C(void *);
void CpuSet(const void *, void *, u32);

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
    u32 has;
};
struct AxeLink {
    u8 unknown0[8];
    struct AxeRoutePoint *point;
};
struct AxeContext {
    u8 unknown0[0xF14];
    u32 clearF14[2];
};

static inline u32 CellIndex(s32 x, s32 z)
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
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior, u32 counter)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = counter;
    actor->field2B1 = 1;
}

static inline void SetWords(const void *source, void *destination, u32 control)
{
    CpuSet(source, destination, control);
}

void Axe_UpdateRouteDestinationPhase(struct SceneActorRegistryResetView *actor)
{
    struct AxeContext *context = actor->context;
    u32 zero;
    vu32 tmp;
    struct AxeVec targetStorage;
    struct AxeVec *target;
    struct AxeVec *copy;
    struct AxeRoutePoint *point;
    struct AxeLink *link;
    s16 deltaX;
    s16 deltaZ;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if (((s32)actor->counter2AC) > 60) {
        Axe_SelectRandomRoutePoint(actor);
        target = &targetStorage;
        link = (struct AxeLink *)(((u8 *)actor) + 0x448);
        point = link->point;
        target->x = point->x;
        target->z = point->z;
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            copy = target;
            target->y = CellHeight(target, CellIndex(cellX, cellZ));
        }
        zero = 0;
        Axe_SetMovementEndpoints(actor, copy);
        sub_080FAA5C(actor);
        SetState(actor, 0, 0, 1, zero);
        {
            struct AxeVec *position = (struct AxeVec *)(&actor->body.position);
            target = &targetStorage;
            target->x -= position->x;
            copy->y -= position->y;
            copy->z -= position->z;
        }
        deltaZ = copy->z;
        deltaX = target->x;
        *(((u8 *)actor) + 0x3cd) = Math_Atan2(deltaX, deltaZ);
        {
            u32 *clear = context->clearF14;
            tmp = zero;
            SetWords((void *)(&tmp), clear, 0x05000002);
        }
    } else
        actor->counter2AC++;
}
