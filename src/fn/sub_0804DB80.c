#include "staged_effect.h"
#define StagedEffect_PlayShakeSequence sub_0804DB80
// MATCHING_CONCESSION: register-pinning
void StagedEffect_PlayShakeSequence(struct StagedEffect *effect)
{
    u32 frame = effect->frame;
    u32 nextFrame;
    if (frame - 10 <= 19 || frame - 60 <= 19) {
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
    } else {
        effect->shakeX = 0;
        effect->shakeZ = 0;
    }
    if (effect->frame == 10 || effect->frame == 20 || effect->frame == 60 || effect->frame == 70)
        sub_0822B2F8(0xD3);
    nextFrame = effect->frame;
    if (nextFrame > 89) {
        register u32 state asm("r1") = 7;
        u16 *destination = &effect->state;
        nextFrame = 0;
        *destination = state;
    } else
        nextFrame++;
    effect->frame = nextFrame;
}
