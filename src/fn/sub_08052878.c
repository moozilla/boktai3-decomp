#include "stacked_actor.h"
void StackedActor_EmitFallingParticles(struct StackedActor *actor)
{
    u32 next_2;
    u32 frame = actor->frame;
    u32 shifted = frame << 16;
    if ((shifted >> 16) <= 31) {
        u32 mask = 3;
        mask &= frame;
        if (!mask) {
            StackedActor_InitFallingParticle(&actor->particles[shifted >> 18], &actor->world);
        }
    }
    {
        u32 next = actor->frame + 1;
        actor->frame = next;
        next_2 = (u16)next;
        if (next_2 == 64) {
            StackedActor_SetUpdate(actor, StackedActor_RetractToIdle);
        }
    }
}
