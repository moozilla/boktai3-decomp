#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"

/* What a target reference points at: an actor's block at +0x50 (position at +0x5C), or the
 * player record SceneActor_FindNearbyPlayerTarget returns. */
struct IvyTargetRef {
    u8 unknown0[0xC];
    struct ContactPosition position;
};

struct IvyTargetRef *SceneActor_FindNearbyPlayerTarget(struct ChaseActor *);
struct ChaseActor *SceneActorRegistry_FindNearestCategoryActor(const struct ContactPosition *);

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
/* Row stride of the current terrain map, in cells. */
#define MAP_WIDTH (*(s16 *)(gTerrainMap->cells + 4))

static inline struct IvyTargetRef *Keep(struct ChaseActor *actor, struct IvyTargetRef *target)
{
    actor->base.tracking.target = (u32)target;
    return target;
}
static inline u32 CellIndex(const struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

/* Ivy target search (tentative): the nearer of SceneActor_FindNearbyPlayerTarget's candidate and the nearest
 * category actor on the same level within the distance limit; it must stand on the terrain
 * cell next to the actor (mode 1: any side, turning to face it; otherwise the side it faces).
 * Stores the result at +0xDC. */
struct IvyTargetRef *Ivy_FindAdjacentTarget(struct ChaseActor *actor, s32 mode)
{
    s32 best = actor->base.tracking.limit;
    struct ChaseActor *other;
    struct IvyTargetRef *target = SceneActor_FindNearbyPlayerTarget(actor);
    struct ContactPosition delta;
    u32 cell;
    if (target && actor->base.body.position.y == target->position.y) {
        s32 distance;
        Delta(&delta, &actor->base.body.position, &target->position);
        distance = delta.x * delta.x + delta.z * delta.z;
        if (best > distance)
            best = distance;
        else
            target = 0;
    }
    other = SceneActorRegistry_FindNearestCategoryActor(&actor->base.body.position);
    if (other && actor->base.body.position.y == other->base.body.position.y) {
        s32 distance;
        Delta(&delta, &actor->base.body.position, &other->base.body.position);
        distance = delta.x * delta.x + delta.z * delta.z;
        if (best > distance)
            target = (void *)((u8 *)other + 0x50);
    }
    if (!target)
        return 0;
    cell = CellIndex(&target->position);
    if (mode == 1) {
        u32 here = actor->base.terrainCell;
        if (here + 1 == cell) {
            actor->base.body.direction = 0;
            return Keep(actor, target);
        }
        if (here - 1 == cell) {
            actor->base.body.direction = 0x80;
            return Keep(actor, target);
        }
        if (here + MAP_WIDTH == cell) {
            actor->base.body.direction = 0x40;
            return Keep(actor, target);
        }
        if (here - MAP_WIDTH == cell) {
            actor->base.body.direction = 0xC0;
            return Keep(actor, target);
        }
        goto fail;
    } else {
        switch (((actor->base.body.direction + 0x20) >> 6) & 3) {
            case 0:
                if (actor->base.terrainCell + 1 != cell)
                    goto fail;
                return Keep(actor, target);
            case 1:
                if (actor->base.terrainCell + MAP_WIDTH != cell)
                    goto fail;
                return Keep(actor, target);
            case 2:
                if (actor->base.terrainCell - 1 != cell)
                    goto fail;
                return Keep(actor, target);
            case 3:
                if (actor->base.terrainCell - MAP_WIDTH != cell)
                    goto fail;
                return Keep(actor, target);
        }
        goto fail;
    }
fail:
    return 0;
}
