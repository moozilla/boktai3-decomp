#include "objects/warp_circle/warp_circle.h"
void WarpCircle_EmitFallingParticles(struct WarpCircle *actor)
{
    u32 next_2;
    u32 frame = actor->frame;
    u32 shifted = frame << 16;
    if ((shifted >> 16) <= 31) {
        u32 mask = 3;
        mask &= frame;
        if (!mask) {
            WarpCircle_InitFallingParticle(&actor->particles[shifted >> 18], &actor->world);
        }
    }
    {
        u32 next = actor->frame + 1;
        actor->frame = next;
        next_2 = (u16)next;
        if (next_2 == 64) {
            WarpCircle_SetUpdate(actor, WarpCircle_RetractToIdle);
        }
    }
}
