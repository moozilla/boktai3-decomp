#define Terrain_ClearTrap Trap_Init
#include "global.h"
#include "engine.h"
#include "ram.h"
void Trap_Init(void *);
s32 Terrain_ResetMap(void)
{
    s32 i;
    struct EngineMap *map = gTerrainMap;
    if (!map)
        return 0;
    map->cells = 0;
    map->resource8 = 0;
    map->boxes = 0;
    map->routes = 0;
    map->boxList = 0;
    gTerrainTrapSerial = 0;
    map->trapCount = 0;
    for (i = 0; i < 64; i++) {
        Trap_Init(&gTerrainMap->traps[i]);
        gTerrainMap->trapTops[i] = 0;
    }
    return 1;
}
