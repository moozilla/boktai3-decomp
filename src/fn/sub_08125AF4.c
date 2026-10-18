#include "global.h"

extern u32 gUnk_02000148;

void SceneTerrainProbe_OnPoolDestroy(void)
{
    gUnk_02000148 = 0;
}
