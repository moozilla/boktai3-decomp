#include "global.h"
#include "engine_globals.h"
struct H {
    u8 p[4];
    s16 w;
    s16 h;
};
struct A {
    u8 p[4];
    struct H *f4;
};
void Terrain_UpdateSize(void)
{
    gTerrainWidth = ((struct A *)gTerrainMap)->f4->w;
    gTerrainHeight = ((struct A *)gTerrainMap)->f4->h;
}
