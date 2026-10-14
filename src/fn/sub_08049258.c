#include "tether_actor.h"
void *sub_08066DB8(void *, void *, u32, u32, u32, u32, u32, u32);
void TetherActor_ApplyPendingEffect(struct TetherActor *actor)
{
    if (actor->pendingEffect) {
        actor->pendingEffect = 0;
        actor->pendingEffectValue = 0;
        actor->pendingEffectHandle =
            sub_08066DB8(actor->pendingEffectHandle, actor->unknown44, 127, 0, 0, 0, 250, 290);
    }
}
