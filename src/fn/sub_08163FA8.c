#include "global.h"
#include "render.h"

void Hud_InitSelectionBounds(u32 a, u32 b)
{
    gBgFadeColor = b;
    gBgFadeBankMask = 0xFFFF;
    gBgFadeLevel = a;
    gObjFadeColor = b;
    gObjFadeFlags = 0;
    gObjFadeLevel = a;
}
