#include "staged_effect.h"
void StagedEffect_PlayShakeSequence(struct StagedEffect *effect)
{
    u32 frame = effect->frame;
    if (frame - 10 <= 19 || frame - 60 <= 19) {
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
    } else {
        effect->shakeX = 0;
        effect->shakeZ = 0;
    }
    if (effect->frame == 10 || effect->frame == 20 || effect->frame == 60 || effect->frame == 70)
        Sound_PlaySong(0xD3);
    if (effect->frame > 89) {
        u32 state = 7;
        effect->state = state;
        effect->frame = 0;
    } else
        effect->frame++;
}
