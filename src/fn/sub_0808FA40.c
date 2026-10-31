#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"

struct SlimeWanderContext {
    u8 unknown0[0x3EC];
    struct ContactPosition target;
};

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

static inline s32 Slime_QuarterDirection(s32 heading)
{
    if (heading >= 0)
        return heading >> 6;
    return -(-heading >> 6);
}

static inline u16 Slime_RandomDirection(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index);
}

static inline s32 Slime_MapWidth(struct EngineMap **terrain)
{
    return ((struct SceneActorChaseTerrainGrid *)(*terrain)->cells)->width;
}

static inline s32 Slime_ForwardCell(struct SceneActorRegistryResetView *actor, s32 steps)
{
    s32 cell = actor->terrainCell;
    struct ContactPosition *position = &actor->body.position;
    struct EngineMap **cachedMapSlot = &gTerrainMap;
    u32 height = SceneActorChase_HeightAt(cell, position);
    u32 shape = SceneActorChase_CellProperties(cell)->attributes >> 4;
    s32 direction = Slime_QuarterDirection(actor->body.direction + 32) & 3;
    s32 next;
    s32 i;
    struct EngineMap **completionMapSlot;
    switch (direction) {
        case 0:
            next = cell + 1;
            i = 0;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next++;
                } while (++i < steps);
            return next - 1;
        case 1:
            next = cell + Slime_MapWidth(cachedMapSlot);
            i = 0;
            completionMapSlot = (struct EngineMap **)0x030052F4;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto south;
                    next += Slime_MapWidth(cachedMapSlot);
                } while (++i < steps);
            return next - Slime_MapWidth(completionMapSlot);
        case 2:
            next = cell - 1;
            i = 0;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next--;
                } while (++i < steps);
            return next + 1;
        case 3:
            next = cell - Slime_MapWidth(cachedMapSlot);
            i = 0;
            completionMapSlot = (struct EngineMap **)0x030052F4;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto north;
                    next -= Slime_MapWidth(cachedMapSlot);
                } while (++i < steps);
            return next + Slime_MapWidth(completionMapSlot);
        default:
            goto home;
    }
south:
    return next - Slime_MapWidth(cachedMapSlot);
north:
    return next + Slime_MapWidth(cachedMapSlot);
home:
    return actor->terrainCell;
}

void Slime_SelectWanderTarget(struct SceneActorRegistryResetView *actor, u32 direction)
{
    struct ContactPosition center;
    struct SlimeWanderContext *context = actor->context;
    s32 tries, chosen;
    actor->body.direction = Slime_RandomDirection();
    tries = 0;
    do {
        chosen = Slime_ForwardCell(actor, 2);
        if (chosen != actor->terrainCell)
            goto selected;
        actor->body.direction += 64;
        tries++;
    } while (tries <= 3);
selected:
    actor->body.direction = direction;
    SceneActorTerrain_GetCenter(chosen, &center);
    context->target = center;
}
