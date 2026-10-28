#include "stacked_actor.h"
#include "player.h"
void PlayerCue_Motion7Variant3(u8 *, u32, u32);
void StackedActor_EmitRisingParticles(struct StackedActor *actor)
{
    u32 next_2;
    u32 frame;
    u32 shifted;
    if (actor->frame == 16) {
        PlayerCue_Motion7Variant3((*(u8 **)gPlayers), 4, 0);
    }
    frame = actor->frame;
    shifted = frame << 16;
    if ((shifted >> 16) <= 31) {
        u32 mask = 3;
        mask &= frame;
        if (!mask) {
            StackedActor_InitRisingParticle(&actor->particles[shifted >> 18], &actor->world);
        }
    }
    {
        u32 next = actor->frame + 1;
        actor->frame = next;
        next_2 = (u16)next;
        if (next_2 > 32) {
            StackedActor_SetUpdate(actor, StackedActor_WaitForRetraction);
        }
    }
}
