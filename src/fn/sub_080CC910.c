#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
/* Golem: route back to its home cell; near it, settle (sub_080C47DC), else face the cell centre; then go to state 2 (tentative) */
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
void Golem_BeginChargeBuildup(void);
void Golem_UpdateRouteMotion(void);
void sub_080C47B8(struct ContactPosition *);
void sub_080C47DC(void *);
s32 Math_Atan2(s32, s32);
struct GolemRoutePoint {
    s16 x, z;
};
struct GolemRouteLink {
    u8 unknown0[8];
    struct GolemRoutePoint *point;
};
static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
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
void Golem_ReturnHome(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u8 *body = actor->context;
    struct ContactPosition center;
    struct ContactPosition diff;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_BeginChargeBuildup;
        SetControl(actor, 0x25, target);
        actor->direction = actor->fieldE9;
        actor->field2B0 = 0;
        *(u16 *)(body + 0x688) = 0;
    }
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->field2B0) {
        struct ContactPosition *dest = *(struct ContactPosition **)(group + 0x200);
        struct GolemRouteLink *link = (struct GolemRouteLink *)(group + 0x74);
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(dest, CellIndex(dest));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        body[0x68D] = 0;
        *(u16 *)(body + 0x680) = 0x78;
        *(u16 *)(body + 0x686) = 0;
        *(u16 *)(body + 0x684) = 2;
        *(u16 *)(body + 0x682) = 0;
        *(struct ContactPosition *)(body + 0x69C) = *(struct ContactPosition *)(body + 0x694);
        SceneActorTerrain_GetCenter(actor->terrainCell, &center);
        sub_080C47B8(&center);
        SceneActorTerrain_Subtract(&diff, &actor->position, &center);
        if (diff.x * diff.x + diff.z * diff.z < 0x400)
            sub_080C47DC(body);
        else {
            u8 angle;
            *(struct ContactPosition *)(*(u8 **)(group + 0x1FC) + 0x1C) = center;
            SceneActorTerrain_Subtract(&diff, &center, &actor->position);
            angle = ((Math_Atan2(diff.x, diff.z) + 0x20) & 0xFF) >> 6 << 6;
            actor->direction = angle;
            body[0x68E] = angle;
        }
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        {
            u32 phase = 2;
            actor->state2A8 = 0;
            actor->phase2A9 = phase;
            actor->behavior2AA = 1;
        }
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    } else
        actor->counter2AC++;
}
