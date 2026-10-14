#include "tether_actor.h"
void *sub_08066DB8(void *, void *, u32, u32, u32, u32, u32, u32);
void TetherActor_ApplyPendingEffect(struct TetherActor *actor)
{
    if (actor->chargeReady) {
        actor->chargeReady = 0;
        actor->charge = 0;
        actor->pendingEffectHandle =
            sub_08066DB8(actor->pendingEffectHandle, &actor->bodyConfig, 127, 0, 0, 0, 250, 290);
    }
}
