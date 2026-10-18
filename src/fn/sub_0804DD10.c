#include "staged_effect.h"
void Sprite_Remove(struct StagedEffectSprite *);
void SceneMessages_UnregisterReceiver(void *);
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
        SceneMessages_UnregisterReceiver((u8 *)effect + 0x18);
    return 0;
}
