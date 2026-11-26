#include "objects/warp_circle/warp_circle.h"
void WarpCircle_RequestRetraction(void)
{
    if (gWarpCircle)
        WarpCircle_SetUpdate(gWarpCircle, WarpCircle_RetractToIdle);
}
