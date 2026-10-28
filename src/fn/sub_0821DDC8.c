#include "global.h"
#include "runtime.h"
struct A {
    u8 p[0xc];
    void *fc;
};
extern struct A *gTerrainMap;
void *ScriptProc_Start(u32, u16);
u32 Terrain_LoadBoxesResource(u16 v)
{
    gTerrainMap->fc = ScriptProc_Start(0xDCFB, v);
    return gTrapDispatchStarted = 0;
}
