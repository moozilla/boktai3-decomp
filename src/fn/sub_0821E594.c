#include "global.h"
#include "ram.h"
struct A {
    u8 p[0x10];
    void *f10;
};
void *ScriptProc_Start(u32, u16);
u32 Terrain_LoadPathsResource(u16 v)
{
    ((struct A *)gTerrainMap)->f10 = ScriptProc_Start(0xD4FB, v);
    return 0;
}
