#include "global.h"
struct A {
    u8 p[0x14];
    void *f14;
};
extern struct A *gTerrainMap;
void Terrain_SetNavMeshes(void *v)
{
    gTerrainMap->f14 = v;
}
