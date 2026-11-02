#include "global.h"
#include "engine.h"

void *ScriptProc_Start(u16, u16);

/* Install the requested navigation archive or clear the map slot if the resource is missing. */
u32 sub_0821F15C(u16 id)
{
    void *navigation = ScriptProc_Start(0xF63B, id);
    if (navigation)
        gTerrainMap->navigation = navigation;
    else
        gTerrainMap->navigation = NULL;
    return 0;
}
