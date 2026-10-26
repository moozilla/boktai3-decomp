#include "global.h"
#include "render.h"

void ScanlineEffect_SetSource(u32 a, u32 b, u32 c)
{
    gScanlineEffectLayer = a;
    gScanlineEffectMode = b;
    gScanlineEffectSource = c;
}
