#include "objects/warp_circle/warp_circle.h"
void WarpCircle_WaitForPlayerRelease(struct WarpCircle *p)
{
    p->frame++;
    if (p->frame > 0x3b)
        WarpCircle_SetUpdate(p, WarpCircle_EmitRisingParticles);
}
