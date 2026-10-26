#include "global.h"
#include "ram.h"
#define gHudManager gFieldHud
extern u16 gUnk_020005A0, gUnk_020005A8;
void Hud_ResetGlobals(void)
{
    gFieldHud = 0;
    gSceneHudMode = 1;
    gUnk_020005A0 = 0;
    gUnk_020005A8 = 1;
}
