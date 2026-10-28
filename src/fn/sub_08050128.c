#include "cell_gate.h"
s32 CellGate_TestPlayerCell(struct CellGate *gate)
{
    s32 x = gPlayers->position.x >> 8;
    s32 z = gPlayers->position.z >> 8;
    u32 cell;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    if (cell == gate->cell)
        return 1;
    return 0;
}
