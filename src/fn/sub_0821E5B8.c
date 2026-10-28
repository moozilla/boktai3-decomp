#include "global.h"
struct A {
    u8 p[0x10];
    void *f10;
};
extern struct A *gTerrainMap;
void Terrain_SetPaths(void *v)
{
    gTerrainMap->f10 = v;
}
