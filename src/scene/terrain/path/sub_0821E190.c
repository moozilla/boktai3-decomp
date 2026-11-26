#include "objects/warp_circle/engine_map.h"
#include "global.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
void Trap_Register(struct TerrainTrap *trap, u8 *top)
{
    s32 i, found, j;
    if (gTerrainMap->trapCount > 63)
        return;
    if (!gTerrainTrapSerial)
        gTerrainTrapSerial = 1;
    trap->serial = gTerrainTrapSerial++;
    found = 0;
    i = gTerrainMap->trapCount;
    while (i > 0) {
        if (trap->id == gTerrainMap->traps[i - 1].id) {
            found = 1;
            break;
        }
        i--;
    }
    if (!found) {
        gTerrainMap->traps[gTerrainMap->trapCount] = *trap;
        gTerrainMap->trapTops[gTerrainMap->trapCount] = top;
    } else {
        for (j = gTerrainMap->trapCount; j > i; j--) {
            gTerrainMap->traps[j] =
                gTerrainMap->traps[j - 1];
            gTerrainMap->trapTops[j] = gTerrainMap->trapTops[j - 1];
        }
        gTerrainMap->traps[i] = *trap;
        gTerrainMap->trapTops[i] = top;
    }
    gTerrainMap->trapCount++;
}
