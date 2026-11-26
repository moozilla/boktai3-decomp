#include "objects/warp_circle/warp_circle.h"
void WarpCircle_RequestRetraction(void)
{
    if (gStackedActor)
        WarpCircle_SetUpdate(gStackedActor, WarpCircle_RetractToIdle);
}
