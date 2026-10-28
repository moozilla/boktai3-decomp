#include "global.h"
#include "runtime.h"
#include "ram.h"
struct A {
    u8 p[0xc];
    void *fc;
};
void *ScriptProc_Start(u32, u16);
u32 Terrain_LoadBoxesResource(u16 v)
{
    ((struct A *)gTerrainMap)->fc = ScriptProc_Start(0xDCFB, v);
    return gTrapDispatchStarted = 0;
}
