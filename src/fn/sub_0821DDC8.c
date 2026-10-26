#include "global.h"
struct A {
    u8 p[0xc];
    void *fc;
};
extern struct A *gTerrainMap;
extern u32 gUnk_03001684;
void *ScriptProc_Start(u32, u16);
u32 Terrain_LoadBoxesResource(u16 v)
{
    gTerrainMap->fc = ScriptProc_Start(0xDCFB, v);
    return gUnk_03001684 = 0;
}
