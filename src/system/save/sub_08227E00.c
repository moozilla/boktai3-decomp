#include "global.h"
#include "engine/video/render.h"
u32 Script_GetKeywordValue(u32, u32);
void Palette_SetGrayscaleDurationFromScript(void)
{
    gGrayscaleControl = Script_GetKeywordValue(0x74, 0);
}
