#include "global.h"
#include "render.h"

void CircleWindow_ResetState(void)
{
    gScanlineDmaSource = 0;
    gScanlineDmaTarget = 0;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 0;
}
