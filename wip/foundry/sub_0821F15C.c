#define Terrain_LoadNavigationResource sub_0821F15C
#define gTerrainMap                    gUnk_030052F4
#include "global.h"
#include "engine.h"
s32 sub_0821F15C(u16 id)
{
    gUnk_030052F4->navigation = ScriptProc_Start(0xF63B, id);
    return 0;
}
