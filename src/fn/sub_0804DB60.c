#include "staged_effect.h"
void StagedEffect_WaitBetweenShakes(struct StagedEffect *effect)
{
    if (effect->frame > 89) {
        u32 state = 6;
        effect->state = state;
        effect->frame = 0;
    } else
        effect->frame++;
}
