#include "staged_effect.h"
#define StagedEffect_WaitForScript sub_0804D910
s32 sub_0804D8AC(struct StagedEffect *);
void StagedEffect_WaitForScript(struct StagedEffect *effect)
{
    if (sub_0804D8AC(effect)) {
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
        sub_0822B2F8(0x1C6);
    }
}
