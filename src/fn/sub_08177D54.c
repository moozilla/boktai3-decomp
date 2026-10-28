#include "global.h"
#include "ram.h"
#define gHudManager gFieldHud
void Hud_ResetGlobals(void)
{
    gFieldHud = 0;
    gSceneHudMode = 1;
    gInventoryUseFlags = 0;
    gHudResetFlag = 1;
}
