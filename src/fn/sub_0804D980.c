#include "staged_effect.h"
// MATCHING_CONCESSION: register-pinning
#define StagedEffect_ShakeAndReveal sub_0804D980
void StagedEffect_ShakeAndReveal(struct StagedEffect *effect)
{
    u32 phase;
    u32 nextFrame;
    if (effect->frame <= 3) {
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
    }
    phase = effect->frame & 3;
    if (phase == 0)
        sub_0804D890(effect);
    else if (phase == 2)
        sub_0804D874(effect);
    nextFrame = effect->frame;
    if (nextFrame > 15) {
        effect->sprites[0].flags &= ~1;
        {
            register u32 state asm("r1") = 3;
            u16 *destination = &effect->state;
            nextFrame = 0;
            *destination = state;
        }
    } else
        nextFrame++;
    effect->frame = nextFrame;
}
