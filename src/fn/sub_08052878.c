// MATCHING_CONCESSION: register-pinning
#include "stacked_actor.h"
#define StackedActor_EmitFallingParticles sub_08052878
void StackedActor_EmitFallingParticles(struct StackedActor *actor)
{
    u32 frame = actor->frame;
    u32 shifted = frame << 16;
    if ((shifted >> 16) <= 31) {
        u32 mask = 3;
        mask &= frame;
        if (!mask)
            sub_0805228C(&actor->particles[shifted >> 18], &actor->world);
    }
    {
        register u32 next asm("r0") = actor->frame + 1;
        actor->frame = next;
        next = (u16)next;
        if (next == 64)
            sub_08052628(actor, sub_080528D0);
    }
}
