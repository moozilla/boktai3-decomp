#include "global.h"

#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline s32 Width(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
static inline s32 Height(void)
{
    return *(s16 *)(gTerrainMap->cells + 6);
}
static inline u8 *Cell(s32 cell)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return override + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
static inline u32 Attributes(s32 cell)
{
    return *(u16 *)(Cell(cell) + 2);
}
static inline u8 IsLevelOpen(s32 cell, u32 level)
{
    if ((*Cell(cell) & 15) != level || (Attributes(cell) & 0x43))
        return 0;
    return 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor)
{
    void *callback = SceneActorControl_OnIdle;
    u8 kind = 0;
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
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u8 RequestPath(struct SceneActorRegistryResetView *actor, void *route,
                             struct ContactPosition *pos)
{
    struct ContactPosition *from = &actor->position;
    s32 r;
    if (!Nav_IsPositionInMesh(route, from, pos)) {
        u16 *flags = &actor->flags2D4;
        s32 mask = ~0x200;
        *flags &= mask;
        return 0;
    }
    r = Terrain_ApplyBlocked(route, from, pos);
    if (r) {
        if (r != 2)
            Memory_Clear((u8 *)actor + 0x64, 8);
        return 1;
    }
    {
        u16 *flags = &actor->flags2D4;
        s32 mask = ~0x200;
        *flags &= mask;
    }
    return 0;
}

// confidence=low: scans to the last open cell in the current facing and starts a route.
u8 Mummy_RequestStraightRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    u8 *nav = (u8 *)actor + 0x3D4;
    s32 cell, i, col, dest, total;
    u32 level;
    s32 dir;
    struct ContactPosition pos;
    u32 result;

    if (FIELD(state, 0x6D2, u16))
        return 0;
    dir = ((actor->direction + 0x20) >> 6) & 3;
    cell = actor->terrainCell;
    level = *Cell(cell) & 15;
    total = Width() * Height();
    switch (dir) {
        case 0:
            i = cell + 1;
            col = Mod(i, Width());
            for (; col < Width(); i++, col++) {
                if (!IsLevelOpen(i, level)) {
                    dest = i - 1;
                    goto found;
                }
            }
            break;
        case 1:
            for (i = cell + Width(); total > i; i += Width()) {
                if (!IsLevelOpen(i, level)) {
                    dest = i - Width();
                    goto found;
                }
            }
            break;
        case 2:
            i = cell - 1;
            col = Mod(i, Width());
            for (; col >= 0; i--, col--) {
                if (!IsLevelOpen(i, level)) {
                    dest = i + 1;
                    goto found;
                }
            }
            break;
        case 3:
            for (i = cell - Width(); i >= 0; i -= Width()) {
                if (!IsLevelOpen(i, level)) {
                    dest = i + Width();
                    goto found;
                }
            }
            break;
    }
    dest = cell;
found:
    if (dest == actor->terrainCell)
        return 0;
    SceneActorTerrain_GetCenter(dest, &pos);
    pos.y = actor->position.y;
    result = RequestPath(actor, FIELD(nav, 0x1FC, void *), &pos);
    if (!result) {
        SetControl(actor);
        SetAnimation(actor, 3, 0, 0);
        return 0;
    }
    FIELD(state, 0x6D2, u16) = 0xF0;
    return 1;
}
