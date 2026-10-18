#define Terrain_ResetMap   sub_0821BB34
#define Terrain_ClearTrap Trap_Init
#define gTerrainMap        gUnk_030052F4
#define gTerrainTrapSerial gUnk_03005300
#include "global.h"
#include "engine.h"
extern u32 gUnk_03005300;
void Trap_Init(void *);
s32 sub_0821BB34(void)
{
    s32 i;
    struct EngineMap *map = gUnk_030052F4;
    if (!map)
        return 0;
    map->cells = 0;
    map->resource8 = 0;
    map->boxes = 0;
    map->routes = 0;
    map->boxList = 0;
    gUnk_03005300 = 0;
    map->trapCount = 0;
    for (i = 0; i < 64; i++) {
        Trap_Init(&gUnk_030052F4->traps[i]);
        gUnk_030052F4->trapTops[i] = 0;
    }
    return 1;
}
