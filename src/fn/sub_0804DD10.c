#include "staged_effect.h"
#define StagedEffect_Destroy sub_0804DD10
void Sprite_Remove(struct StagedEffectSprite *);
void sub_08020D08(void *);
s32 StagedEffect_Destroy(struct StagedEffect *effect)
{
    struct StagedEffectSprite *sprite = effect->sprites;
    s32 i = 4;
    do {
        Sprite_Remove(sprite);
        sprite++;
        i--;
    } while (i >= 0);
    if (effect->hasScript)
        sub_08020D08((u8 *)effect + 0x18);
    return 0;
}
