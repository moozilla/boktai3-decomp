#include "global.h"
#include "ram.h"
struct A {
    u8 p[0x14];
    void *f14;
};
void Terrain_SetNavMeshes(void *v)
{
    ((struct A *)gTerrainMap)->f14 = v;
}
