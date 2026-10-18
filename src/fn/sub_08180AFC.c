#define BouncingEffect_RemoveSprite sub_08180AFC
#include "global.h"
struct BouncingEffectSprite {
    u32 flags;
};
struct BouncingEffect {
    u8 unknown0[8];
    struct BouncingEffectSprite sprite;
};
void Sprite_UnlinkFromList(struct BouncingEffectSprite *, u32);
void sub_08180AFC(struct BouncingEffect *effect)
{
    effect->sprite.flags |= 1;
    Sprite_UnlinkFromList(&effect->sprite, 0);
}
