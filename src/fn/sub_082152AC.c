#include "global.h"
#include "render.h"


void ObjGfx_ResetAllocationCounters(void)
{
    gDynamicTileSum = 0;
    gMainSpriteTileSum = 0;
    gObjTileCursor = 0;
    if (gSpriteTileResource != 0)
        gObjTileCursor = gStaticTileCount;
}
