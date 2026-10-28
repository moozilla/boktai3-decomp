#include "global.h"
#include "ram.h"
struct A {
    u8 p[4];
    void *f4;
};
void Terrain_SetNeighborOffsets(void);
void Terrain_BuildRowOffsets(void);
void Terrain_UpdateSize(void);
void Terrain_SetGrid(void *v)
{
    ((struct A *)gTerrainMap)->f4 = v;
    Terrain_SetNeighborOffsets();
    Terrain_BuildRowOffsets();
    Terrain_UpdateSize();
}
