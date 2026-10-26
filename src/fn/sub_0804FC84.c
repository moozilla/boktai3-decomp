#include "repeated_effect.h"
void Callback_InvokeActor(struct RepeatedEffect *, void (*)(struct RepeatedEffect *));
void RepeatedEffect_TickEntry(struct RepeatedEffect *effect)
{
    if (effect->constrainCell) {
        if (effect->cell == *(u32 *)((u8 *)gGameState + 0x5A0)) {
            if (!effect->constrained)
                effect->collider.flags &= 0xFFFB;
            Callback_InvokeActor(effect, effect->update);
        } else {
            u8 constrainedValue = effect->constrained;
            u8 *constrained = &effect->constrained;
            if (constrainedValue) {
                u8 state = effect->state;
                if (state == 0) {
                    RepeatedEffect_SetState(effect, 0, RepeatedEffect_TickNeutral);
                    effect->sprite.frame = state;
                    effect->flashing = 0;
                    effect->request = 0;
                } else if (state == 2) {
                    RepeatedEffect_SetState(effect, 2, RepeatedEffect_TickUpperState);
                    effect->sprite.frame = 8;
                    effect->transitionHold = 16;
                } else if (state == 1) {
                    RepeatedEffect_SetState(effect, 2, RepeatedEffect_TickLowerState);
                    effect->sprite.frame = 5;
                    effect->transitionHold = 16;
                }
                effect->collider.flags |= 4;
                *constrained = 0;
            }
        }
    } else
        Callback_InvokeActor(effect, effect->update);
    if (effect->collider.cooldown)
        effect->collider.cooldown--;
}
