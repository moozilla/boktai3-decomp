#include "global.h"
struct LinkedEffectCollider {
    u8 unknown[6];
    u16 flags;
    u8 tail[0x4C];
};
struct LinkedEffect {
    u8 unknown[0x58];
    struct LinkedEffectCollider colliders[2];
    u8 pad100[0x76];
    u8 state;
    u8 pad177[3];
    u8 active;
};
struct EffectVariantSource {
    u8 unknown[0x38];
    u32 flags;
    u8 pad3C[6];
    u8 variant;
};
void LinkedEffect_SetVariant(void *, u32);
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void MarkCollider(struct LinkedEffectCollider *sprite)
{
    sprite->flags |= 4;
}
void LinkedEffect_ApplyVariant(struct EffectVariantSource *source, void *unused, struct LinkedEffect *effect)
{
    if (TestFlags(&source->flags, 0x20)) {
        u8 *sprite = (u8 *)effect + 0x1C;
        LinkedEffect_SetVariant(sprite, source->variant);
        LinkedEffect_SetVariant(sprite, source->variant);
        MarkCollider(&effect->colliders[0]);
        MarkCollider(&effect->colliders[1]);
        effect->state = 3;
        effect->active = 1;
    }
}
