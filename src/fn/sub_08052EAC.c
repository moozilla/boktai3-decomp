// MATCHING_CONCESSION: register-pinning
#include "stacked_actor.h"
extern u8 *gUnk_02000580;
void sub_0815B9CC(u8 *, u32, u32);
void StackedActor_EmitRisingParticles(struct StackedActor *actor)
{
    u32 frame;
    u32 shifted;
    if (actor->frame == 16)
        sub_0815B9CC(gUnk_02000580, 4, 0);
    frame = actor->frame;
    shifted = frame << 16;
    if ((shifted >> 16) <= 31) {
        u32 mask = 3;
        mask &= frame;
        if (!mask)
            sub_080521DC(&actor->particles[shifted >> 18], &actor->world);
    }
    {
        register u32 next asm("r0") = actor->frame + 1;
        actor->frame = next;
        next = (u16)next;
        if (next > 32)
            StackedActor_SetUpdate(actor, StackedActor_WaitForRetraction);
    }
}
