#include "engine/shared/global.h"
#include "engine/shared/render.h"

void Palette_SetBackdropColor(u16 v)
{
    gBackdropColor = v;
    gBackdropOverride = 1;
}
