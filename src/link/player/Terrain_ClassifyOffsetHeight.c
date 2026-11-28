#include "global.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "scene/terrain/terrain_sampling.h"

void SceneVec_OffsetPolar(struct SceneVec *out, const struct SceneVec *in, s32 angle, s32 distance);
s32 Player_GetTerrainHeightAtPosition(const struct TerrainCell *, struct SceneVec *);

s32 Terrain_ClassifyOffsetHeight(struct SceneVec *origin, s32 angle, s32 distance)
{
    struct SceneVec pos;
    s32 index;
    struct TerrainCell *cell;
    u8 *override;

    SceneVec_OffsetPolar(&pos, origin, angle, distance);
    index = Terrain_GetCellIndex((struct ContactPosition *)&pos);
    if (gTerrainMap->cells == NULL)
        cell = NULL;
    else
        cell = &((struct TerrainGrid *)gTerrainMap->cells)->cells[index];
    if (Player_GetTerrainHeightAtPosition(cell, &pos) > pos.y)
        return 1;
    override = TerrainModifiers_FindCellOverride(index, 1);
    if (override != NULL && Player_GetTerrainHeightAtPosition(
                                &((struct TerrainCellOverride *) override)->cell, &pos) > pos.y)
        return 2;
    return 0;
}
