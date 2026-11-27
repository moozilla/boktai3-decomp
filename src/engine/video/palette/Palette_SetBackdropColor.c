#include "global.h"
#include "engine/video/render.h"

void Palette_SetBackdropColor(u16 v)
{
    gBackdropColor = v;
    gBackdropOverride = 1;
}
