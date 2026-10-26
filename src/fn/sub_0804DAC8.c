#include "staged_effect.h"
void Camera_RequestShake(u32);
void StagedEffect_BlendAndShake(struct StagedEffect *effect)
{
    u16 *random = (u16 *)RANDOM_TABLE_ADDR;
    u32 *new_var;
    u32 index = (gRandomIndex + 1) & 0x3FF;
    u32 sample = random[index];
    u32 mask = 31;
    sample &= mask;
    sample -= 16;
    effect->shakeX = sample;
    index = (index + 1) & 0x3FF;
    gRandomIndex = index;
    new_var = &index;
    *(&sample) = random[*new_var];
    sample &= mask;
    sample -= 16;
    effect->shakeZ = sample;
    {
        u32 frame = effect->frame;
        if (frame <= 3) {
            Camera_RequestShake(40);
            StagedEffect_BlendPalette(effect, 10, 8, effect->frame << 4);
        } else
            StagedEffect_BlendPalette(effect, 8, 10, (frame - 4) << 4);
    }
    if (effect->frame > 7) {
        effect->shakeX = 0;
        effect->shakeZ = 0;
        {
            u32 state = 5;
            u16 *destination = &effect->state;
            *destination = state;
        }
        effect->frame = 0;
    } else
        effect->frame++;
}
