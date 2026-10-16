#include "tether_actor.h"
void TetherActor_UpdateChargeDecay(struct TetherActor *);
void TetherActor_UpdateInteractionCooldown(struct TetherActor *);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, void *, u32, u32, u32);
void TetherActor_UpdateChargeAndCooldown(struct TetherActor *actor)
{
    if (!(gUnk_030053F4 & 512)) {
        TetherActor_UpdateChargeDecay(actor);
        TetherActor_UpdateInteractionCooldown(actor);
        if (actor->contactCooldown) {
            actor->chargeCollider.flags = 4 | actor->chargeCollider.flags;
            --actor->contactCooldown;
        } else
            actor->chargeCollider.flags = 0xFFFB & actor->chargeCollider.flags;
    } else
        actor->pendingEffectHandle = ActorAttachment_UpdateFromDescriptor(
            actor->pendingEffectHandle, &actor->bodyConfig, 127, 0, 0, 0, 250, 290);
}
