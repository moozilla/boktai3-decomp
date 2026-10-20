#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 81.6%
// Cardinal scan tail layout, retry-loop hoisting, and route argument allocation still differ.
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
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
    s32 result = sub_0821F204(route, position, target);
    if (!result) {
        ClearRouteBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = sub_0821F7A8(route, position, target);
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

static inline s32 RoundDirection(s32 n)
{
    if (n >= 0)
        return n >> 6;
    return -((-n) >> 6);
}
// confidence=low: tests cardinal cells at decreasing ranges.
u8 sub_0812E118(u32, u32, u32, u32);
static inline s32 Sweep(struct SceneActorRegistryResetView *actor, s32 range)
{
    s32 cell = actor->terrainCell;
    u32 height = CellHeight(&actor->position, cell);
    u32 shape = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 direction = RoundDirection(actor->direction + 32) & 3;
    s32 next, i;
    switch (direction) {
        case 0:
            next = cell + 1;
            i = 0;
            if (i < range)
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next++;
                } while (++i < range);
            return next - 1;
        case 1:
            next = cell + SceneActorTerrain_Width();
            i = 0;
            if (i < range)
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next += SceneActorTerrain_Width();
                } while (++i < range);
            return next - SceneActorTerrain_Width();
        case 2:
            next = cell - 1;
            i = 0;
            if (i < range)
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next--;
                } while (++i < range);
            return next + 1;
        case 3:
            next = cell - SceneActorTerrain_Width();
            i = 0;
            if (i < range)
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next -= SceneActorTerrain_Width();
                } while (++i < range);
            return next + SceneActorTerrain_Width();
        default:
            return actor->terrainCell;
    }
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
            dest = Sweep(actor, range);
            if (dest != actor->terrainCell) {
                found = 1;
                goto nextRange;
            }
            actor->direction += 64;
        } while (++tries <= 3);
    nextRange:
        range--;
        actor->direction = savedDirection;
    } while (!found && range > 0);
    SceneActorTerrain_GetCenter(dest, &target);
    group = (u8 *)&actor->group;
    if (TryRoute(actor, group, &target))
        return 1;
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Vorn_SelectReachableEscapeCell);
#endif
