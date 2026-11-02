#include "game_state.h"
#include "repeated_effect.h"
void RepeatedEffect_TickNeutral(struct RepeatedEffect *effect)
{
    if (effect->request) {
        effect->flashing = 1;
        effect->request = 0;
        if (effect->stateFrame > 63)
            effect->stateFrame = ((effect->stateFrame - 64) & 31) >> 1;
        else
            effect->stateFrame &= 15;
    }
    if (effect->flashing) {
        u16 frame = ++effect->stateFrame;
        if (frame > 95) {
            effect->sprite.frame = 2;
            effect->flashing = 0;
            effect->stateFrame = 0;
        } else {
            if (effect->stateFrame <= 63) {
                if ((effect->stateFrame & 15) <= 7)
                    effect->sprite.frame = 1;
                else
                    effect->sprite.frame = 2;
            } else {
                if ((effect->stateFrame & 31) <= 15)
                    effect->sprite.frame = 1;
                else
                    effect->sprite.frame = 2;
            }
        }
    }
    {
        s32 intensity = gGameState->unk876;
        if (intensity == 0) {
            if (effect->behaviorFlags & 2) {
                effect->balance--;
                if (effect->balance < 0)
                    effect->balance = intensity;
            } else {
                effect->balance--;
                if (effect->balance < -300)
                    RepeatedEffect_SetState(effect, 1, RepeatedEffect_TickLowerTransition);
            }
        } else if (intensity > 7) {
            if (effect->behaviorFlags & 1) {
                effect->balance++;
                if (effect->balance > 0)
                    effect->balance = 0;
            } else {
                effect->balance++;
                if (effect->balance > 300)
                    RepeatedEffect_SetState(effect, 2, RepeatedEffect_TickUpperTransition);
            }
        }
    }
}
