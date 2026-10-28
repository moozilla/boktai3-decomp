#include "global.h"
#include "render.h"

void Video_SetRenderCallbacks(u16 a, u32 b, u32 c, u32 d)
{
    gCamera.mode = a;
    gProjectedSpriteCallback = b;
    gSpriteRenderCallback = c;
    gExtraRenderCallback = d;
}
