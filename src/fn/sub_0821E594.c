#include "global.h"
struct A {
    u8 p[0x10];
    void *f10;
};
extern struct A *gTerrainMap;
void *ScriptProc_Start(u32, u16);
u32 Terrain_LoadPathsResource(u16 v)
{
    gTerrainMap->f10 = ScriptProc_Start(0xD4FB, v);
    return 0;
}
