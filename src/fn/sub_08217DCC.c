#include "global.h"
#include "render.h"

void Palette_SetBackdropColor(u16 v)
{
    gBackdropColor = v;
    gBackdropOverride = 1;
}
