#ifndef GUARD_EFFECT_PART_H
#define GUARD_EFFECT_PART_H

#include "scene_particle_step.h"

struct EffectPartManager {
    u8 unknown0[0x18];
    const u16 *graphics;
};

struct EffectPartGroup {
    u8 unknown0[2];
    u8 active, count;
};

struct EffectPart {
    u8 state, phase;
    u16 timer;
    s16 lifetime;
    u8 unknown6[0xA];
    struct SceneParticleEffect sprite;
    struct DisplaySequenceState sequence;
};

static inline u32 EffectPart_StepSequence(struct SceneParticleEffect *sprite, const u16 *graphics,
                                          struct DisplaySequenceState *sequence)
{
    return SceneParticle_StepSprite(sequence, sprite, graphics);
}

#endif
