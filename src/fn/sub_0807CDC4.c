// MATCHING_CONCESSION: register-pinning
#include "scene_actor_position_search.h"
#include "scene_actor_attachment.h"
#include "scene_actor_terrain.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
/* Candidate keys match the search order; vector pointer allocation still differs. */
s32 Nav_IsPositionInMesh(void *, const struct ContactPosition *, const struct ContactPosition *);
static inline s32 DistanceSquared(struct ContactPosition *difference,
                                  const struct ContactPosition *a, const struct ContactPosition *b)
{
    s32 x, z, xSquare, zSquare;
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
    register struct ContactPosition *delta asm("r8") = &difference;
    s32 adjacent;
    struct SceneActorEffectCandidate *candidate;
    typeof(&center) t1;
    {
        u8 k1 = 0xC3;
        if ((!((*((u16 *)(SceneActorTerrain_GetCell(cell) + 2))) & k1)) &&
            Nav_IsPositionInMesh(context, &source->position, input)) {
            *output = *input;
            return;
        }
    }
    adjacent = cell + 1;
    SceneActorTerrain_GetCenter(adjacent, &center);
    candidates[0].key = DistanceSquared_FindReachableNeighbor(delta, &center, &source->position);
    candidates[0].value = adjacent;
    adjacent = cell + SceneActorTerrain_Width();
    do {
        SceneActorTerrain_GetCenter(adjacent, &center);
    } while (0);
    candidates[1].key = DistanceSquared_FindReachableNeighbor(delta, &center, &source->position);
    candidates[1].value = cell + SceneActorTerrain_Width();
    adjacent = cell - 1;
    SceneActorTerrain_GetCenter(adjacent, &center);
    candidates[2].key = DistanceSquared_FindReachableNeighbor(delta, &center, &source->position);
    candidates[2].value = adjacent;
    adjacent = cell - SceneActorTerrain_Width();
    SceneActorTerrain_GetCenter(adjacent, &center);
    candidates[3].key = DistanceSquared_FindReachableNeighbor(delta, &center, &source->position);
    candidates[3].value = cell - SceneActorTerrain_Width();
    SceneActorEffect_SortCandidates(candidates, 4);
    candidate = candidates;
    do {
        if (!((*((u16 *)(SceneActorTerrain_GetCell(candidate->value) + 2))) & 0xC3)) {
            t1 = &center;
            SceneActorTerrain_GetCenter(candidate->value, t1);
            if (Nav_IsPositionInMesh(context, &source->position, &center)) {
                *output = center;
                return;
            }
        }
        candidate++;
    } while (candidate <= (&candidates[3]));
    output->x = 30000;
    output->y = 0;
    output->z = 30000;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorAttachment_FindReachableNeighbor);
#endif
