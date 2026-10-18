#define ActorEffect_UpdateExpiry  sub_081B6F68
#define ActorEffect_RemoveSprites sub_081B69F4
#include "global.h"
struct EffectCollision {
    u8 pad0[6];
    u16 flags;
};
struct ActorEffect {
    u8 entered;
    u8 pad1[5];
    u16 timer;
    u8 pad8[0xBC - 8];
    struct EffectCollision collision;
};
void sub_081B69F4(void *, struct ActorEffect *, s32);
void sub_081B6A38(struct ActorEffect *);
static inline void EffectCollision_Disable(struct EffectCollision *collision)
{
    collision->flags |= 4;
}
void sub_081B6F68(void *owner, struct ActorEffect *effect, s32 index)
{
    if (effect->entered) {
        effect->entered = 0;
        EffectCollision_Disable(&effect->collision);
    }
    sub_081B6A38(effect);
    if (effect->timer > 29)
        sub_081B69F4(owner, effect, index);
    else
        effect->timer++;
}
