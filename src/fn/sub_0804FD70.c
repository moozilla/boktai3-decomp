#include "repeated_effect.h"
#define RepeatedEffect_InitParameters sub_0804FD70
s32 sub_0822C034(struct RepeatedEffectPosition *);
void RepeatedEffect_InitParameters(struct RepeatedEffect *effect, u16 tag,
                                   struct RepeatedEffectPosition *position, u8 state, u32 behavior,
                                   u32 constrain)
{
    effect->tag = tag;
    effect->state = state;
    effect->transitionHold = 0;
    effect->flashing = 0;
    effect->request = 0;
    effect->behaviorFlags = *(u8 *)&behavior;
    effect->constrainCell = constrain;
    if ((u8)constrain)
        effect->cell = sub_0822C034(position);
}
