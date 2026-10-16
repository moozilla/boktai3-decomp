#include "repeated_effect.h"
s32 Region_FindContaining(struct RepeatedEffectPosition *);
void RepeatedEffect_InitParameters(struct RepeatedEffect *effect, u32 tag,
                                   struct RepeatedEffectPosition *position, u32 state, u32 behavior,
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
        effect->cell = Region_FindContaining(position);
}
