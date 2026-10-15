#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetState(struct TetherActor *, u32);
void *sub_08066DB8(void *, struct TetherBodyConfig *, u32, u32, u16 *, u32, u32, u32);
void *sub_08065C50(void *, u32, u32);
void TetherActor_UpdateDescendingState(struct TetherActor *actor)
{
    u16 *frame;
    register u32 raw asm("r0");
    register u32 incremented asm("r1");
    register u32 current asm("r0");
    u32 zero;
    if (actor->state != 6)
        TetherActor_SetState(actor, 6);
    zero = gUnk_030053F4 & 512;
    if (zero)
        return;
    actor->glowMode = zero;
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
            *handle = sub_08066DB8(*handle, &actor->bodyConfig, 1, 0, value, 200, 250, 290);
        }
    } else {
        current = (u16)incremented;
        if (current == 6) {
            void **handle = &actor->pendingEffectHandle;
            *handle = sub_08065C50(*handle, 1, actor->bodyConfig.id);
        }
        actor->displayOffsetY -= 30;
        if (*frame > 8)
            actor->destroyRequested = 1;
    }
}
