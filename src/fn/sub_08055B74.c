#include "stage_icon_display.h"
void StageIconDisplay_RequestKill(void)
{
    if (gUnk_0200011C)
        gUnk_0200011C->killRequested = 1;
}
