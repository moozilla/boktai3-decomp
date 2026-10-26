#include "global.h"
#include "engine.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, s32);
struct FloorPoint {
    s16 x, y, z, pad;
};
union FloorPointWords {
    struct FloorPoint point;
    struct {
        u32 x : 16;
        u32 y : 16;
        u32 z : 16;
    } words;
};
static inline s32 cellIndex(struct FloorPoint *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 groundHeight(struct FloorPoint *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
/* Script: floor height of the terrain cell at world (x, z), slope included;
   stores it into the result variable and returns it. */
s32 Terrain_GetFloorHeightFromScript(void)
{
    union FloorPointWords pos;
    s32 height;
    u32 result[2];
    pos.words.x = Script_GetValue();
    pos.words.y = 0;
    pos.words.z = Script_GetValue();
    height = groundHeight(&pos.point);
    Script_ReadReference(result);
    Script_WriteReferenceValue(result, 0, height);
    return height;
}
