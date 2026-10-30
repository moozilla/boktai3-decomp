#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

void Axe_SelectRandomRoutePoint(void *);
void Axe_BeginMoveToPosition(void *, void *);
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
    u32 has;
};
struct AxeContext {
    u8 unknown0[0xE4C];
    u32 flagsE4C;
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
static inline u32 Flag(void *base, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)base + off) & mask : *(u32 *)((u8 *)base + off) & mask;
}
static inline u32 IsDown(struct SceneActorRegistryResetView *actor)
{
    u32 r;
    if (actor && (Flag(actor, 0x40, 0x2D4) || Flag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        r = 1;
    else
        r = 0;
    asm("" : "+r"(r));
    return r;
}
static inline u32 HasMark(struct AxeContext *context)
{
    u32 r;
    if (Flag(context, 0x10, 0xE4C)) {
        r = 1;
        asm("" : "+r"(r));
    } else
        r = 0;
    return r;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Axe_UpdateRouteWander(struct SceneActorRegistryResetView *actor)
{
    struct AxeContext *context = actor->context;
    struct AxeGroup *group = (void *)&actor->group.common;
    struct AxeVec targetStorage;
    struct AxeVec *target;
    struct AxeRoutePoint *point;
    u32 has;
    if (IsDown(actor))
        return;
    if (Flag(actor, 0x100, 0x2D4))
        return;
    has = HasMark(context);
    asm("" : "+r"(has));
    if (has == 0) {
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
