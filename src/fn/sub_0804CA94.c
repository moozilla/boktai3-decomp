#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetState(struct TetherActor *, u32);
void *ActorAttachment_UpdateFromDescriptor(void *, struct TetherBodyConfig *, u32, u32, u16 *, u32,
                                           u32, u32);
void *ActorAttachment_ReleaseResource(void *, u32, u32);
void TetherActor_UpdateRapidDescendingState(struct TetherActor *actor)
{
    u16 *frame;
    register u32 raw asm("r0");
    register u32 incremented asm("r1");
    register u32 current asm("r0");
    u32 zero;
    if (actor->state != 7)
        TetherActor_SetState(actor, 7);
    {
        u16 *glow = &actor->glowMode;
        zero = 0;
        *glow = zero;
    }
    frame = &actor->stateFrame;
    raw = *frame;
    incremented = raw + 1;
    *frame = incremented;
    current = (u16)incremented;
    if (current <= 5) {
        current = (u16)incremented;
        if (current == 1) {
            u16 *value = &actor->effectValue;
            void **handle;
            *value = zero;
            handle = &actor->pendingEffectHandle;
            *handle = ActorAttachment_UpdateFromDescriptor(*handle, &actor->bodyConfig, 1, 0, value,
                                                           200, 250, 290);
        }
    } else {
        current = (u16)incremented;
        if (current == 6) {
            void **handle = &actor->pendingEffectHandle;
            *handle = ActorAttachment_ReleaseResource(*handle, 1, actor->bodyConfig.id);
        }
        actor->displayOffsetY -= 60;
        if (*frame > 29)
            actor->destroyRequested = 1;
    }
}
