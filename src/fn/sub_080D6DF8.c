#include "global.h"
/* SceneActorPath_FaceAndCheckStraightPath: faces the actor toward its target and checks that the grid cells along the row/column from its cell to the goal cell are passable (SceneActorTerrain_CanTraverseCell); restores the facing on failure (provisional) */
#include "scene_actor_terrain.h"
u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, u32);
s32 Math_Atan2(s32, s32);

struct Unk080D6DF8Goal {
    u8 unknown0[6];
    u16 cell;
};
struct Unk080D6DF8Target {
    u8 unknown0[0xC];
    struct ContactPosition position;
    u8 unknown14[8];
    struct Unk080D6DF8Goal *goal;
};
struct Unk080D6DF8Actor {
    u8 unknown0[0x59];
    u8 direction;
    u8 unknown5A[2];
    struct ContactPosition position;
    u8 unknown64[0x78];
    struct Unk080D6DF8Target *target;
    u8 unknownE0[0x214];
    u16 terrainCell;
};

struct Unk080D6DF8Delta {
    s16 x, y, z;
};
static inline s32 GridWidth(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
static inline s32 GridHeight(void)
{
    return *(s16 *)(gTerrainMap->cells + 6);
}

static inline u8 CanReach(s32 cell, s32 goal, s32 y, u32 kind, s32 total)
{
    s32 i, col;
    if (cell < goal) {
        i = cell + 1;
        for (col = Mod(i, GridWidth()); col < GridWidth() && SceneActorTerrain_CanTraverseCell(cell, i, y, kind);
             i++, col++) {
            if (i == goal)
                return 1;
        }
        for (i = cell + GridWidth(); total > i && SceneActorTerrain_CanTraverseCell(cell, i, y, kind);
             i += GridWidth()) {
            if (i == goal)
                return 1;
        }
    } else {
        i = cell - 1;
        for (col = Mod(i, GridWidth()); col >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i--, col--) {
            if (i == goal)
                return 1;
        }
        for (i = cell - GridWidth(); i >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i -= GridWidth()) {
            if (i == goal)
                return 1;
        }
    }
    return 0;
}

static inline u8 FindPath(struct Unk080D6DF8Actor *actor, s32 goal)
{
    s32 cell = actor->terrainCell;
    s32 y = actor->position.y;
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = GridWidth() * GridHeight();
    return CanReach(cell, goal, y, kind, total);
}

u8 SceneActorPath_FaceAndCheckStraightPath(struct Unk080D6DF8Actor *actor)
{
    struct Unk080D6DF8Target *target = actor->target;
    s32 goal = target->goal->cell;
    u8 saved = actor->direction;
    struct Unk080D6DF8Delta delta;
    u8 result;

    {
        struct Unk080D6DF8Delta *d = &delta;
        struct ContactPosition *from = &actor->position;
        struct ContactPosition *to = &target->position;
        d->x = to->x - from->x;
        d->y = to->y - from->y;
        d->z = to->z - from->z;
    }
    actor->direction = Math_Atan2(delta.x, delta.z);
    if (goal == actor->terrainCell)
        result = 1;
    else if (!FindPath(actor, goal)) {
        actor->direction = saved;
        result = 0;
    } else
        result = 1;
    return result;
}
