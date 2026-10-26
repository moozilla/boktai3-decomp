#include "global.h"
#include "ram.h"

void SceneTerrainProbe_OnPoolDestroy(void)
{
    gPairSlotPool = 0;
}
