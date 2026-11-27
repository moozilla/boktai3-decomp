#include "player/effects/spiral_trail.h"
u32 SpiralTrail_Destroy(struct SpiralTrailTask *p)
{
    SpiralTrail_RemoveSprite(p);
    if (p->mode == 1)
        SpiralTrail_RemoveParticles(p);
}
