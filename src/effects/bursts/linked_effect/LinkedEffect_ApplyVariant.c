#include "effects/bursts/linked_effect.h"
#include "engine/word_flags.h"

struct EffectVariantSource {
    u8 unk_00[0x38];
    u32 flags;
    u8 unk_3C[6];
    u8 variant;
};

void LinkedEffect_SetVariant(void *, u32);

void LinkedEffect_ApplyVariant(struct EffectVariantSource *source, void *unused,
                               struct LinkedEffect *effect)
{
    if (WordFlags_Test(&source->flags, 0x20)) {
        struct ContactCollider *colliders;

        LinkedEffect_SetVariant(&effect->sprite.position, source->variant);
        LinkedEffect_SetVariant(&effect->sprite.position, source->variant);
        colliders = effect->colliders;
        colliders->flags |= 4;
        colliders++;
        colliders->flags |= 4;
        effect->state = 3;
        effect->entered = 1;
    }
}
