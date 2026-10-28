#include "global.h"
#include "ram.h"
struct A {
    u8 p[0x10];
    void *f10;
};
void Terrain_SetPaths(void *v)
{
    ((struct A *)gTerrainMap)->f10 = v;
}
