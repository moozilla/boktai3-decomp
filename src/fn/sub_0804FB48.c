// MATCHING_CONCESSION: register-pinning
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
        u32 frame = effect->stateFrame + 1;
        effect->stateFrame = frame;
        if ((u16)frame > 95) {
            effect->sprite.frame = 2;
            effect->flashing = 0;
            effect->stateFrame = 0;
        } else {
            u32 displayedFrame;
            asm("" : "+r"(frame));
            if ((u16)frame <= 63) {
                if ((frame & 15) > 7)
                    goto frameTwo;
                displayedFrame = 1;
                goto storeFrame;
            } else {
                if ((frame & 31) > 15)
                    goto frameTwo;
                displayedFrame = 1;
                asm("" : "+r"(displayedFrame));
                goto storeFrame;
            frameTwo:
                displayedFrame = 2;
            storeFrame:
                effect->sprite.frame = displayedFrame;
            }
        }
    }
    {
        s32 intensity = *(s16 *)((u8 *)gGameState + 0x876);
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
