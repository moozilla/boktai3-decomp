#include "global.h"
void TimeDisplay_RemoveAnimatedObjects(void *);
void Bg_ClearTilemap(s32);
void Params3D0_DestroyDisplay(void *display)
{
    TimeDisplay_RemoveAnimatedObjects(display);
    Bg_ClearTilemap(2);
}
