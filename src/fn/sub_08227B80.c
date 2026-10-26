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
        u32 x;
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
/* Script: second halfword of the terrain cell record at world (x, z);
   stores it into the result variable and returns it. */
s32 Terrain_GetCellWordFromScript(void)
{
    union FloorPointWords pos;
    s32 index;
    u8 *cell;
    u32 kind;
    u32 result[2];
    u32 mask;
    u16 v = Script_GetValue();
    u16 v2;
    mask = 0xFFFF0000;
    cell = (u8 *)(u32)v;
    ((u32 *)&pos)[0] = (u32)cell;
    v2 = Script_GetValue();
    ((u32 *)&pos)[1] = (((u32 *)&pos)[1] & mask) | v2;
    index = cellIndex(&pos.point);
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *(u16 *)(cell + 2);
    Script_ReadReference(result);
    Script_WriteReferenceValue(result, 0, kind);
    return kind;
}
