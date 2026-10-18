#include "global.h"

void PaletteFrames_CaptureColors(void *);

s32 PaletteFrames_Initialize(void *p)
{
    PaletteFrames_CaptureColors(p);
    return 0;
}
