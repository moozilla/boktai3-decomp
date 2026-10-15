#include "stage_icon_display.h"
#define StageIconDisplay_RequestKill sub_08055B74
void StageIconDisplay_RequestKill(void)
{
    if (gUnk_0200011C)
        gUnk_0200011C->killRequested = 1;
}
