#include "global.h"
struct A {
    u8 p[4];
    void *f4;
};
extern struct A *gTerrainMap;
void *ScriptProc_Start(u32, u16);
void Terrain_SetNeighborOffsets(void);
void Terrain_BuildRowOffsets(void);
void Terrain_UpdateSize(void);
u32 Terrain_LoadGridResource(u16 v)
{
    gTerrainMap->f4 = ScriptProc_Start(0xAE1B, v);
    Terrain_SetNeighborOffsets();
    Terrain_BuildRowOffsets();
    Terrain_UpdateSize();
    return 0;
}
