#include "global.h"
#include "engine.h"
void Terrain_BuildRowOffsets(void)
{
    s32 width = *(s16 *)(gTerrainMap->cells + 4);
    u16 *row = gTerrainMap->rows;
    s32 offset = 0;
    s32 i = 0;
    for (; i < *(s16 *)(gTerrainMap->cells + 6); i++) {
        row[i] = offset;
        offset += width;
    }
}
