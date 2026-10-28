#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
#ifdef NONMATCHING
// fuzzy: 92.3%
// fix.py best version (reprinted C)
// confidence=low: inherited route/reaction interpretation from static callers.

// confidence=low: route role inferred from callers and terrain/path tests.

// Inline actor/group lifetimes, random-mask ordering, and register allocation still differ.
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

static inline void ClearRouteBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, u8 *group,
                          struct ContactPosition *target)
{
    void *route = *(void **)(group + 0x1fc);
    struct ContactPosition *position = &actor->position;
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        ClearRouteBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->motion, 8);
        return 1;
    }
    ClearRouteBits(&actor->flags2D4, 0x200);
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 b)
{
    u32 behavior = b;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

// confidence=low: distance thresholds select recovery routes.
extern const u8 gUnk_0824E230[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);

u8 Vorn_SelectReachableEscapeCell(void *, u32);
u32 SceneActor_DispatchContextAction(void *, u32);
static inline u32 RandomBits(u32 mask)
{
    u32 value;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    value = *(const u16 *)((gRandomIndex * 2) + (u32)gRandomTable);
    return value & mask;
}
static inline u32 PlayerFlags(u8 *player, u32 mask)
{
    return *(u32 *)(player + 0x20) & mask;
}
static inline u8 SelectRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    struct ContactPosition *dest = actor->state5D4;
    struct RouteLink {
        u8 pad[8];
        u16 *point;
    };
    struct RouteLink *point = (struct RouteLink *)(group + 0x74);
    u32 cell;
    struct ContactPosition target;
    struct ContactPosition **slot = (void *)((u8 *)actor + 0xdc);
    dest->x = point->point[0];
    dest->z = point->point[1];
    dest->y = CellHeight(dest, CellIndex(dest));
    cell = CellIndex((struct ContactPosition *)((u8 *)*slot + 12));
    if (cell == actor->terrainCell || PlayerFlags(((u8 **)gPlayers)[0], 0x20000)) {
        SceneActor_DispatchContextAction(actor->context, 0);
        return 0;
    }
    SceneActorTerrain_GetCenter(cell, &target);
    if (!TryRoute(actor, group, &target)) {
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
        return 0;
    }
    return 1;
}
u32 Vorn_SelectRetreatRoute(struct SceneActorRegistryResetView *actor, u32 reset)
{
    u8 *body = actor->context;
    u8 *group = (u8 *)(&actor->group);
    u8 old;
    typeof(old) c1;
    if ((!reset) && (*((u16 *)(body + 0x6d0))))
        return 0;
    *((u16 *)(body + 0x6d0)) = 60;
    {
        u32 *field = &actor->flags2CC;
        s32 mask = ~0x10;
        *field &= mask;
    }
    {
        u16 bit = 0x10;
        struct Flags {
            s16 bits : 16;
        };
        struct Flags *flags = (void *)(body + 0x6d4);
        flags->bits |= bit;
        flags->bits &= ~0x40;
    }
    SceneActorReaction_ResetResponseState(group + 0x8c, gUnk_0824E230, 0x486);
    (*((u32 *)(body + 0x6c4)))++;
    if (actor->distanceF0 < (*((u32 *)(body + 0x6c8)))) {
        old = actor->direction;
        actor->direction = actor->fieldE9 + 0x80;
        if (!Vorn_SelectReachableEscapeCell(actor, old))
            return 0;
        SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SetState(actor, 0x15);
    } else if (actor->distanceF0 > (*((u32 *)(body + 0x6cc)))) {
        old = actor->direction;
        {
            u32 random = RandomBits(0xffffffff);
            actor->direction = (random & 63) + actor->fieldE9;
        }
        if (!actor->fieldDC)
            actor->fieldDC = (u32)(((u8 **)gPlayers)[0] + 0x24);
        if ((!SelectRoute(actor)) && (!Vorn_SelectReachableEscapeCell(actor, old)))
            return 0;
        SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SetState(actor, 0x15);
    } else {
        old = actor->direction;
        c1 = old;
        {
            u32 random = RandomBits(0xffffffff);
            actor->direction = (random & 63) + actor->fieldE9;
        }
        if (!actor->fieldDC)
            actor->fieldDC = (u32)(((u8 **)gPlayers)[0] + 0x24);
        if ((!SelectRoute(actor)) && (!Vorn_SelectReachableEscapeCell(actor, c1)))
            return 0;
        SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SetState(actor, 0x15);
    }
    {
        u32 bit = 4;
        u32 *field = &actor->flags2D0;
        u32 value = *field;
        value |= bit;
        *field = value;
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", Vorn_SelectRetreatRoute);
#endif
