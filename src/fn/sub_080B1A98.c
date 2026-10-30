#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
// confidence=low: route role inferred from callers and terrain/path tests.

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
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                          struct ContactPosition *position, struct ContactPosition *target)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        ClearRouteBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
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

static inline u8 Test(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
extern const u8 gUnk_0824E230[];

// confidence=low: routes toward the current target after a reaction.

u8 SceneActor_DispatchContextAction(void *, u32);
void sub_080B3110(void *, u32, const void *, u32);
static inline u32 MotionFlags(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2D4 & mask;
}
static inline u8 TestContext(struct SceneActorRegistryResetView *p, u32 mask)
{
    if (p->flags6D4 & mask)
        return 1;
    return 0;
}
static inline u8 NeedsResponse(struct SceneActorRegistryResetView *p)
{
    if (TestContext(p, 1) || p->field67C >= 0)
        return 1;
    return 0;
}
void Vorn_BeginTargetPursuit(struct SceneActorRegistryResetView *actor)
{
    u8 *group;
    struct ContactPosition *dest;
    struct RouteLink {
        u8 pad[8];
        u16 *point;
    };
    struct RouteLink *point;
    ClearRouteBits((void *)((u8 *)actor->context + 0x6d4), 8);
    if (NeedsResponse(actor->context)) {
        sub_080B3110(actor, 1, gUnk_0824E230, 0x29a);
        return;
    }
    if (MotionFlags(actor, 0x100))
        return;
    group = (u8 *)&actor->group.common;
    if (TestContext(actor->context, 0x1d)) {
        actor->field2C2 = 32;
        if (TestContext(actor->context, 4) && MotionFlags(actor, 0x80)) {
            actor->group.common.gate.counter += 300;
            if (actor->group.common.gate.counter > actor->group.common.gate.duration)
                actor->group.common.gate.counter = actor->group.common.gate.duration;
        }
        if (SceneActor_DispatchContextAction(actor->context, 1))
            return;
    }
    {
        u32 state = 3, phase = 1;
        actor->state2A8 = state;
        actor->phase2A9 = phase;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
    SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    {
        u32 mask = 16;
        u32 *p = &actor->flags2CC;
        u32 value = *p;
        value |= mask;
        *p = value;
    }
    actor->field2C2 = 32;
    {
        struct SceneActorReactionCommand *command = (void *)(group + 0x8c);
        void *payload = actor->entryTarget228;
        u32 kind = 7;
        command->duration = 60;
        command->payload = payload;
        command->first = 0;
        command->second = 0;
        command->kind = kind;
    }
    dest = *(struct ContactPosition **)(group + 0x200);
    point = (struct RouteLink *)(group + 0x74);
    dest->x = point->point[0];
    dest->z = point->point[1];
    dest->y = CellHeight(dest, CellIndex(dest));
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!TryRoute(actor, *(void **)(group + 0x1fc), &actor->body.position,
                  &actor->tracking.target->position))
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
