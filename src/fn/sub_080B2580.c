#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
// confidence=low: inherited route/reaction interpretation from static callers.

// confidence=low: route role inferred from callers and terrain/path tests.

#ifdef NONMATCHING
// fuzzy: 86.7%
// fix.py best (sweep after the inlined-insn fix)
// Width-load rematerialization, stack-slot order, and literal-pool placement still differ.
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
    struct ContactPosition *position = &actor->body.position;
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (result && (result = Terrain_ApplyBlocked(route, position, target))) {
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

static inline s32 RoundDirection(s32 n)
{
    if (n >= 0)
        return n >> 6;
    return -((-n) >> 6);
}
// confidence=low: tests cardinal cells at decreasing ranges.
u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);
static inline s32 Sweep(struct SceneActorRegistryResetView *actor, s32 range)
{
    s32 cell = actor->terrainCell;
    u32 height = CellHeight(&actor->body.position, cell);
    u32 shape = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 direction = RoundDirection(actor->body.direction + 32) & 3;
    s32 next, i;
    switch (direction) {
        case 0:
            next = cell + 1;
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next++;
                } while (++i < range);
            goto east;
        case 1:
            next = cell + SceneActorTerrain_Width();
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next += SceneActorTerrain_Width();
                } while (++i < range);
            goto south;
        case 2:
            next = cell - 1;
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next--;
                } while (++i < range);
            goto west;
        case 3:
            next = cell - SceneActorTerrain_Width();
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next -= SceneActorTerrain_Width();
                } while (++i < range);
            goto north;
        default:
            goto home;
    }
east:
    return next - 1;
south:
    return next - SceneActorTerrain_Width();
west:
    return next + 1;
north:
    return next + SceneActorTerrain_Width();
home:
    return actor->terrainCell;
}
static inline u32 CenterHeight(s32 cell)
{
    u32 attr = *SceneActorTerrain_GetCell(cell);
    return (attr & 15) << 8;
}
static inline void GetCenter(s32 cell, struct ContactPosition *p)
{
    s32 x = Mod(cell, SceneActorTerrain_Width());
    s32 z = Div(cell, SceneActorTerrain_Width());
    p->x = (x << 8) + 128;
    p->y = CenterHeight(cell);
    p->z = (z << 8) + 128;
}
static inline s32 Sweep_SelectReachableEscapeCell(struct SceneActorRegistryResetView *actor,
                                                  s32 range)
{
    s32 cell = actor->terrainCell;
    u32 height = CellHeight(&actor->body.position, cell);
    u32 shape = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 direction = RoundDirection(actor->body.direction + 32) & 3;
    s32 next;
    s32 i;
    switch (direction) {
        case 0:
            next = cell + 1;
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next++;
                } while (++i < range);
            goto east;
        case 1:
            next = cell + SceneActorTerrain_Width();
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next += SceneActorTerrain_Width();
                } while (++i < range);
            goto south;
        case 2:
            next = cell - 1;
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next--;
                } while (++i < range);
            goto west;
        case 3:
            next = cell - SceneActorTerrain_Width();
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next -= SceneActorTerrain_Width();
                } while (++i < range);
            goto north;
        default:
            goto home;
    }
east:
    return next - 1;
south:
    return next - SceneActorTerrain_Width();
west:
    return next + 1;
north:
    return next + SceneActorTerrain_Width();
home:
    return actor->terrainCell;
}

u8 Vorn_SelectReachableEscapeCell(struct SceneActorRegistryResetView *actor, u32 savedDirection)
{
    struct ContactPosition target;
    u8 *group;
    u32 found = 0;
    s32 range = 2;
    s32 dest;
    do {
        s32 tries = 0;
        do {
            dest = Sweep_SelectReachableEscapeCell(actor, range);
            if (dest != actor->terrainCell) {
                found = 1;
                goto nextRange;
            }
            actor->body.direction += 64;
        } while ((++tries) <= 3);
    nextRange:
        range--;

        actor->body.direction = savedDirection;
        if (found || (range <= 0))
            goto done;
    } while (1);
done:
    GetCenter(dest, &target);

    group = (u8 *)(&actor->group.common);
    if (!TryRoute(actor, group, &target)) {
        Callback_InvokeActor(actor, *((void **)(group + 0x224)));
        return 0;
    }
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", Vorn_SelectReachableEscapeCell);
#endif
