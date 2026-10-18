#define ActorEffect_RemoveSprites sub_081B69F4
#include "global.h"
struct EffectOwner {
    u8 pad0[0x18];
    u32 activeMask;
};
struct EffectSprite {
    u8 bytes[0x2C];
};
struct EffectCollision {
    u8 pad0[6];
    u16 flags;
};
struct ActorEffect {
    u8 pad0[0xBC];
    struct EffectCollision collision;
    u8 padC4[0x110 - 0xC4];
    struct EffectSprite sprites[6];
};
void Sprite_Remove(void *);
static inline void EffectCollision_Disable(struct EffectCollision *collision)
{
    collision->flags |= 4;
}
u32 sub_081B69F4(struct EffectOwner *owner, struct ActorEffect *effect, u32 index)
{
    struct EffectSprite *sprite = effect->sprites;
    s32 count = 5;
    do {
        Sprite_Remove(sprite);
        count--;
        sprite++;
    } while (count >= 0);
    EffectCollision_Disable(&effect->collision);
    owner->activeMask &= ~(1 << index);
}
