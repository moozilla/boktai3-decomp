// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
#define TetherActor_StartInteractionEffect sub_08049D4C
#define TetherActor_UpdatePendingEffect    sub_080492A4
#define TetherActor_SelectInteractionState sub_080492F0
extern struct TetherActor *gUnk_02000488;
void TetherActor_UpdatePendingEffect(struct TetherActor *);
void TetherActor_SelectInteractionState(struct TetherActor *);
void *sub_08066DB8(void *, void *, u32, u32, void *, u32, u32, u32);
void TetherActor_StartInteractionEffect(void)
{
    struct TetherActor *actor = gUnk_02000488;
    u32 state;
    u32 savedState;
    register u32 zero asm("r3");
    u16 *cooldown;
    register u32 delay asm("r0");
    if (!actor)
        goto done;
    state = actor->state;
    savedState = state;
    asm("" : "+r"(savedState));
    if (savedState == 1) {
        TetherActor_UpdatePendingEffect(actor);
        {
            u16 *lock = &actor->interactionLock;
            zero = 0;
            *lock = savedState;
        }
        cooldown = &actor->interactionCooldown;
        delay = 300;
    } else {
        if (savedState != 0 && savedState != 2 && ((u8)(state - 3) > 1 || !actor->pendingEffect))
            goto done;
        TetherActor_UpdatePendingEffect(actor);
        TetherActor_SelectInteractionState(actor);
        cooldown = &actor->interactionCooldown;
        zero = 0;
        delay = 600;
    }
    *cooldown = delay;
    actor->effectValue = zero;
    actor->pendingEffectHandle = sub_08066DB8(actor->pendingEffectHandle, actor->unknown44, 1, zero,
                                              &actor->effectValue, 200, 250, 330);
done:;
}
