#include "global.h"
#include "render.h"

extern u32 gUnk_030042EC;

void ObjGfx_ResetAllocationCounters(void)
{
    gDynamicTileSum = 0;
    gMainSpriteTileSum = 0;
    gObjTileCursor = 0;
    if (gUnk_030042EC != 0)
        gObjTileCursor = gStaticTileCount;
}
