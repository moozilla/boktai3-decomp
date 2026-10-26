#include "scene_actor_position_search.h"
#include "scene_actor_attachment.h"
#include "scene_actor_terrain.h"

struct NeighborTerrainCell {
    u8 heightAndShape;
    u8 unknown1;
    u16 flags;
};

static inline void Neighbor_GetCenter(struct ContactPosition *position, s32 cell)
{
    s32 x = Mod(cell, SceneActorTerrain_Width());
    s32 z = Div(cell, SceneActorTerrain_Width());
    position->x = ((u32)x << 8) + 128;
    position->y = (*SceneActorTerrain_GetCell(cell) & 15) << 8;
    position->z = ((u32)z << 8) + 128;
}

s32 Nav_IsPositionInMesh(void *, const struct ContactPosition *, const struct ContactPosition *);
static inline s32 DistanceSquared_FindReachableNeighbor(struct ContactPosition *difference,
                                                        const struct ContactPosition *a,
                                                        const struct ContactPosition *b)
{
    s32 x;
    s32 z;
    s32 xSquare;
    s32 zSquare;
    SceneActorTerrain_Subtract(difference, a, b);
    x = difference->x;
    xSquare = x * x;
    z = difference->z;
    zSquare = z * z;
    {
        s32 copy = zSquare;
        return (u32)xSquare + (u32)copy;
    }
}

void SceneActorAttachment_FindReachableNeighbor(const struct SceneActorPositionSource *source,
                                                void *context, struct ContactPosition *output,
                                                const struct ContactPosition *input)
{
    s32 cell = SceneActorTerrain_GetCellIndex(input);
    struct SceneActorEffectCandidate candidates[4];
    struct ContactPosition center;
    struct ContactPosition difference;
    s32 i;
    {
        u8 k1 = 0xC3;
        if ((!(((struct NeighborTerrainCell *)SceneActorTerrain_GetCell(cell))->flags & k1)) &&
            Nav_IsPositionInMesh(context, &source->position, input)) {
            *output = *input;
            return;
        }
    }
    Neighbor_GetCenter(&center, cell + 1);
    candidates[0].key =
        DistanceSquared_FindReachableNeighbor(&difference, &center, &source->position);
    candidates[0].value = cell + 1;
    Neighbor_GetCenter(&center, cell + SceneActorTerrain_Width());
    candidates[1].key =
        DistanceSquared_FindReachableNeighbor(&difference, &center, &source->position);
    candidates[1].value = cell + SceneActorTerrain_Width();
    Neighbor_GetCenter(&center, cell - 1);
    candidates[2].key =
        DistanceSquared_FindReachableNeighbor(&difference, &center, &source->position);
    candidates[2].value = cell - 1;
    Neighbor_GetCenter(&center, cell - SceneActorTerrain_Width());
    candidates[3].key =
        DistanceSquared_FindReachableNeighbor(&difference, &center, &source->position);
    candidates[3].value = cell - SceneActorTerrain_Width();
    SceneActorEffect_SortCandidates(candidates, 4);
    for (i = 0; i <= 3; i++) {
        u16 flags =
            ((struct NeighborTerrainCell *)SceneActorTerrain_GetCell(candidates[i].value))->flags;
        flags &= 0xC3;
        if (!flags) {
            Neighbor_GetCenter(&center, candidates[i].value);
            if (Nav_IsPositionInMesh(context, &source->position, &center)) {
                *output = center;
                return;
            }
        }
    }
    output->x = 30000;
    output->y = 0;
    output->z = 30000;
}
