#include "staged_effect.h"
s32 StagedEffect_CheckScriptCompletion(struct StagedEffect *);
void StagedEffect_WaitForScript(struct StagedEffect *effect)
{
    if (StagedEffect_CheckScriptCompletion(effect)) {
        u16 *state = &effect->state;
        u32 mask = ~1;
        struct StagedEffectSprite *sprite = &effect->sprites[1];
        do {
            sprite->flags &= mask;
            sprite++;
        } while ((s32)sprite <= (s32)&effect->sprites[4]);
        {
            u32 zero = 0;
            *state = 1;
            effect->frame = zero;
        }
        Sound_PlaySong(0x1C6);
    }
}
