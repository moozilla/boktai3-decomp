#include "repeated_effect.h"
#define RepeatedEffect_TickEntry sub_0804FC84
void sub_0824923C(struct RepeatedEffect *, void (*)(struct RepeatedEffect *));
void RepeatedEffect_TickEntry(struct RepeatedEffect *effect)
{
    if (effect->constrainCell) {
        if (effect->cell == *(u32 *)((u8 *)gUnk_02000710 + 0x5A0)) {
            if (!effect->constrained)
                effect->collider.flags &= 0xFFFB;
            sub_0824923C(effect, effect->update);
        } else {
            u8 constrainedValue = effect->constrained;
            u8 *constrained = &effect->constrained;
            if (constrainedValue) {
                u8 state = effect->state;
                if (state == 0) {
                    sub_0804F7D0(effect, 0, sub_0804FB48);
                    effect->sprite.frame = state;
                    effect->flashing = 0;
                    effect->request = 0;
                } else if (state == 2) {
                    sub_0804F7D0(effect, 2, sub_0804F970);
                    effect->sprite.frame = 8;
                    effect->transitionHold = 16;
                } else if (state == 1) {
                    sub_0804F7D0(effect, 2, sub_0804F888);
                    effect->sprite.frame = 5;
                    effect->transitionHold = 16;
                }
                effect->collider.flags |= 4;
                *constrained = 0;
            }
        }
    } else
        sub_0824923C(effect, effect->update);
    if (effect->collider.cooldown)
        effect->collider.cooldown--;
}
