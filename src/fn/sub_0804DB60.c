#include "staged_effect.h"
// MATCHING_CONCESSION: register-pinning
void StagedEffect_WaitBetweenShakes(struct StagedEffect *effect)
{
    register u32 next asm("r0");
    if (effect->frame > 89) {
        u32 state = 6;
        u16 *out = &effect->state;
        next = 0;
        *out = state;
    } else
        next = effect->frame + 1;
    effect->frame = next;
}
