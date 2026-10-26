#include "staged_effect.h"
void StagedEffect_ShakeAndReveal(struct StagedEffect *effect)
{
    u32 phase;
    if (effect->frame <= 3) {
        u16 *random = (u16 *)RANDOM_TABLE_ADDR;
        u32 index = (gRandomIndex + 1) & 0x3FF;
        u32 sample;
        u32 *new_var;
        u32 mask;
        new_var = &index;
        *(&sample) = random[*new_var];
        mask = 31;
        sample &= mask;
        sample -= 16;
        effect->shakeX = sample;
        index = (index + 1) & 0x3FF;
        gRandomIndex = index;
        sample = random[index];
        sample &= mask;
        sample -= 16;
        effect->shakeZ = sample;
    }
    phase = effect->frame & 3;
    if (phase == 0)
        StagedEffect_MarkOuterSpritesAlternate(effect);
    else if (phase == 2)
        StagedEffect_MarkOuterSprites(effect);
    if (effect->frame > 15) {
        effect->sprites[0].flags &= ~1;
        {
            u32 state = 3;
            effect->state = state;
        }
        effect->frame = 0;
    } else
        effect->frame++;
}
