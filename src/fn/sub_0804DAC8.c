#include "staged_effect.h"
// MATCHING_CONCESSION: register-pinning
void sub_082260A4(u32);
void StagedEffect_BlendAndShake(struct StagedEffect *effect)
{
    u16 *random = (u16 *)RANDOM_TABLE_ADDR;
    register u32 index asm("r1") = (gUnk_03005308 + 1) & 0x3FF;
    register u32 sample asm("r0") = random[index];
    u32 mask = 31;
    sample &= mask;
    sample -= 16;
    effect->shakeX = sample;
    index = (index + 1) & 0x3FF;
    gUnk_03005308 = index;
    sample = random[index];
    sample &= mask;
    sample -= 16;
    effect->shakeZ = sample;
    {
        u32 frame = effect->frame;
        if (frame <= 3) {
            sub_082260A4(40);
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
