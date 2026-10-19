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
void ActorEffect_RemoveSprites(void *, struct ActorEffect *, s32);
void ActorEffect_UpdatePositionTrail(struct ActorEffect *);
static inline void EffectCollision_Disable(struct EffectCollision *collision)
{
    collision->flags |= 4;
}
void ActorEffect_UpdateExpiry(void *owner, struct ActorEffect *effect, s32 index)
{
    if (effect->entered) {
        effect->entered = 0;
        EffectCollision_Disable(&effect->collision);
    }
    ActorEffect_UpdatePositionTrail(effect);
    if (effect->timer > 29)
        ActorEffect_RemoveSprites(owner, effect, index);
    else
        effect->timer++;
}
