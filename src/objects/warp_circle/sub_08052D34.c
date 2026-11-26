#include "objects/warp_circle/warp_circle.h"
void WarpCircle_WaitForScriptParticles(struct WarpCircle *p)
{
    p->frame++;
    if (p->frame > 0xf)
        WarpCircle_SetUpdate(p, WarpCircle_EmitScriptParticles);
}
