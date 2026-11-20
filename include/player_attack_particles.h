#ifndef GUARD_PLAYER_ATTACK_PARTICLES_H
#define GUARD_PLAYER_ATTACK_PARTICLES_H

#include "task.h"
#include "scene_animation.h"
#include "sprite.h"
#include "display_sequence.h"

struct AttackParticleSlot {
    u32 life, sequence, mode;
    s32 angle;
};
struct AttackParticleOwner {
    u8 unknown0[0x20];
    struct ContactPosition position;
};
/* PlayerAttackEffect_Create allocates 0x5A8 bytes. Its initializer sets up sixteen slots. */
struct AttackParticleEffect {
    struct Task task;
    struct SceneAnimation graphics[6];
    s32 radius;
    struct DisplaySequenceSprite sprites[16];
    struct DisplaySequenceState sequences[16];
    struct AttackParticleSlot slots[16];
    u32 frame, unknown588;
    const u8 *facing, *side;
    const struct AttackParticleOwner *owner;
    u32 previous, current, element;
    void *sequenceResource;
};

typedef char AttackParticleEffectSizeCheck[sizeof(struct AttackParticleEffect) == 0x5A8 ? 1 : -1];

#endif
