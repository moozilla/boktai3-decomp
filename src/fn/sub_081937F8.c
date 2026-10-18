#include "global.h"

void TerrainModifiers_UnregisterCell(u8 *);

void Nidhoggr_UnregisterTerrainCells(u8 *p)
{
    if (*(p + 0x71BD)) {
        *(p + 0x71BD) = 0;
        TerrainModifiers_UnregisterCell(p + 0x7100);
        TerrainModifiers_UnregisterCell(p + 0x7110);
    }
}
