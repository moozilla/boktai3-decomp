#include "staged_effect.h"
s32 Div(s32, s32);
void AnimatedParticles_EmitRing(void *, u32, u32, u32, u32, u32, u32, u32);
void Trigger_ConsumePending(void *, u32);
void StagedEffect_ContractBurst(struct StagedEffect *effect)
{
    u32 phase;
    u16 *travel;
    u32 value;
    u32 zero;
    effect->scale = Div(effect->travel * 1008, effect->initialTravel) + 1024;
    phase = effect->frame & 3;
    if (phase == 0)
        StagedEffect_MarkOuterSpritesAlternate(effect);
    else if (phase == 2)
        StagedEffect_MarkOuterSprites(effect);
    travel = (u16 *)&effect->travel;
    value = *travel - 70;
    zero = 0;
    *travel = value;
    if ((s32)(value << 16) <= 0) {
        *travel = zero;
        effect->scale = 1024;
        {
            u32 state = 4;
            u16 *destination = &effect->state;
            *destination = state;
        }
        effect->frame = zero;
        Sound_PlaySong(0x1C4);
        StagedEffect_MarkOuterSprites(effect);
        AnimatedParticles_EmitRing((u8 *)effect + 0x90, 8, 1, 1, 64, 12, 40, 250);
        if (effect->hasScript)
            Trigger_ConsumePending((u8 *)effect + 0x18, 1);
    } else
        effect->frame++;
}
