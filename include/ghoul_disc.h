#ifndef GUARD_GHOUL_DISC_H
#define GUARD_GHOUL_DISC_H

#include "ghoul.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_init_primitives.h"

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

static inline u8 Ghoul_TestDiscFlags(struct Ghoul *context, u32 mask)
{
    if (context->discFlags & mask)
        return 1;
    return 0;
}

static inline void Ghoul_ClearDiscFlags(struct Ghoul *context, u32 mask)
{
    context->discFlags &= ~mask;
}

static inline s32 Ghoul_QuarterDirection(s32 angle)
{
    if (angle >= 0)
        return angle >> 6;
    return -(-angle >> 6);
}

static inline s32 Ghoul_Abs(s32 value)
{
    if (value < 0)
        return -value;
    return value;
}

static inline s32 Ghoul_TargetPlayerSlot(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTarget *target = actor->tracking.target;
    if (!target)
        return -1;
    return SceneActorChase_PlayerSlot(target);
}

static inline void Ghoul_CaptureTarget(struct SceneActorRegistryResetView *actor,
                                       struct Ghoul *context, u32 slot)
{
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        s32 player = Ghoul_TargetPlayerSlot(actor);
        if (player >= 0)
            context->discTargets[slot] =
                ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->target.position;
    } else {
        context->discTargets[slot] = actor->tracking.target->position;
    }
}

static inline s32 Ghoul_MapWidth(struct EngineMap **terrain)
{
    return ((struct SceneActorChaseTerrainGrid *)(*terrain)->cells)->width;
}

static inline s32 Ghoul_ForwardCell(struct SceneActorRegistryResetView *actor, s32 steps)
{
    struct EngineMap **terrain;
    u32 cell = actor->terrainCell;
    u32 height = SceneActorChase_HeightAt(cell, &actor->body.position);
    u32 shape = SceneActorChase_CellProperties(cell)->attributes >> 4;
    s32 next, i, chosen, stride;
    switch (Ghoul_QuarterDirection(actor->body.direction + 32) & 3) {
        case 0:
            next = cell + 1;
            i = 0;
            do {
                if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                    break;
                next++;
                i++;
            } while (i < steps);
            chosen = next - 1;
            break;
        case 1:
            terrain = &gTerrainMap;
            next = cell + Ghoul_MapWidth(terrain);
            for (i = 0; i < steps; i++) {
                if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                    goto blockedSouth;
                stride = ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width;
                next += stride;
            }
            chosen = next - stride;
            break;
        case 2:
            next = cell - 1;
            i = 0;
            do {
                if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                    break;
                next--;
                i++;
            } while (i < steps);
            chosen = next + 1;
            break;
        case 3:
            terrain = &gTerrainMap;
            next = cell - Ghoul_MapWidth(terrain);
            for (i = 0; i < steps; i++) {
                if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                    goto blockedNorth;
                stride = ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width;
                next -= stride;
            }
            chosen = next + stride;
            break;
        blockedSouth:
            terrain = &gTerrainMap;
            chosen = next - Ghoul_MapWidth(terrain);
            break;
        blockedNorth:
            terrain = &gTerrainMap;
            chosen = next + Ghoul_MapWidth(terrain);
            break;
        default:
            chosen = actor->terrainCell;
            break;
    }
    return chosen;
}

static inline s32 Ghoul_DirectionDelta(s32 direction, s32 target)
{
    s32 delta = target - direction;
    if (delta > 128)
        delta -= 256;
    else if (delta < -128)
        delta += 256;
    return delta;
}

#endif
