#ifndef GUARD_PLAYER_ATTACK_PARTICLES_H
#define GUARD_PLAYER_ATTACK_PARTICLES_H

#include "engine/task.h"
#include "engine/video/anim_sprite.h"
#include "scene/scene_animation.h"
#include "engine/video/sprite.h"
#include "engine/video/display_sequence.h"

struct AttackParticleSlot {
    u32 life, sequence, mode;
    s32 angle;
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
    const struct AnimSprite *owner;
    u32 previous, current, element;
    void *sequenceResource;
};

typedef char AttackParticleEffectSizeCheck[sizeof(struct AttackParticleEffect) == 0x5A8 ? 1 : -1];

s32 PlayerAttackEffect_AllocateParticle(struct AttackParticleEffect *effect,
                                        struct ContactPosition *position, s32 lifetime,
                                        void *animation, s32 sequence, s32 mode);

#endif
