#include "global.h"
struct H {
    u8 p[4];
    s16 w;
    s16 h;
};
struct A {
    u8 p[4];
    struct H *f4;
};
extern struct A *gTerrainMap;
extern s32 gTerrainWidth, gTerrainHeight;
void Terrain_UpdateSize(void)
{
    gTerrainWidth = gTerrainMap->f4->w;
    gTerrainHeight = gTerrainMap->f4->h;
}
