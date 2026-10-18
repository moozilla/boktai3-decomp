#include "global.h"
void TimeDisplay_RemoveAnimatedObjects(void);
void Bg_ClearTilemap(s32);
void Params3D0_DestroyDisplay(void)
{
    TimeDisplay_RemoveAnimatedObjects();
    Bg_ClearTilemap(2);
}
