#include "global.h"
void TimeDisplay_RemoveAnimatedObjects(void *);
void Bg_ClearTilemap(s32);
void TitleDisplay_Destroy(void *display)
{
    TimeDisplay_RemoveAnimatedObjects(display);
    Bg_ClearTilemap(2);
}
