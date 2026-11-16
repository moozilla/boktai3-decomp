#include "global.h"
#include "axe.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

void Axe_SelectRandomRoutePoint(struct S080FA7D0 *s);
void Axe_BeginMoveToPosition(u8 *s, s16 *v);
void Axe_ClearForcedMovement(void *);
void Axe_ResetMovementFlags(void *);

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
static inline u32 CellIndex(struct AxeVec *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
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
static inline u32 Flag_UpdateRouteWander(void *base, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)base + off) & mask : *(u32 *)((u8 *)base + off) & mask;
}
static inline u8 IsDown(struct ChaseActor *actor)
{
    if (actor &&
        (Flag_UpdateRouteWander(actor, 0x40, 0x2D4) || Flag_UpdateRouteWander(actor, 0x20000, 0x2D0) || actor->base.health <= 0))
        return 1;
    return 0;
}
static inline u8 HasMark(struct Axe *context)
{
    if (Flag_UpdateRouteWander(context, 0x10, 0xE4C))
        return 1;
    return 0;
}
static inline void SetState(struct ChaseActor *a, u8 s, u8 p, u8 b)
{
    a->base.state2A8 = s;
    a->base.phase2A9 = p;
    a->base.behavior2AA = b;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

void Axe_UpdateRouteWander(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    struct AxeGroup *group = (void *)&actor->group.common;
    struct AxeVec targetStorage;
    struct AxeVec *target;
    struct AxeRoutePoint *point;
    if (IsDown(actor))
        return;
    if (Flag_UpdateRouteWander(actor, 0x100, 0x2D4))
        return;
    if (!HasMark(context)) {
        group->field44 = 0;
        return;
    }
    if (group->field44 != 0) {
        group->field44--;
        return;
    }
    Axe_SelectRandomRoutePoint(actor);
    target = &targetStorage;
    point = group->point;
    targetStorage.x = point->x;
    target->z = point->z;
    target->y = CellHeight(target, CellIndex(&targetStorage));
    Axe_BeginMoveToPosition(actor, &targetStorage);
    Axe_ClearForcedMovement(actor);
    Axe_ResetMovementFlags(actor);
    SetState(actor, 0, 0, 2);
}
