#include "global.h"
#include "engine.h"
extern u8 *gUnk_02000580[4];
struct MapInfo {
    u8 pad[0x24];
    u16 rows[1];
};

s32 sub_08158E68(s32 i)
{
    u8 *actor = gUnk_02000580[i];
    u8 *q = *(u8 **)(actor + 0x40);
    s32 x, y;
    if (q != NULL)
        return *(u16 *)(q + 6);
    x = *(s16 *)(actor + 0x30) >> 8;
    y = *(s16 *)(actor + 0x34) >> 8;
    if (x < 0 || y < 0 || x >= gTerrainWidth || y >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[y] + x;
}