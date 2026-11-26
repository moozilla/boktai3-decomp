#ifndef GUARD_SCRIPTED_PARTICLE_H
#define GUARD_SCRIPTED_PARTICLE_H

#include "engine/interrupts_and_video/sprite.h"
#include "engine/shared/display_sequence.h"
#include "engine/shared/task.h"

struct ScriptedParticle {
    struct DisplaySequenceSprite sprite;
    struct SpriteRenderResource graphics;
    struct DisplaySequenceState sequence;
    u8 state, unknown59, angle, unknown5B;
    u16 rise;
    s16 speed;
    u8 unknown60[4];
    u16 riseDelay, unknown66, durationScale;
    u8 unknown6A[2];
    s8 index;
    u8 riseSteps;
    u16 hiddenFrames;
};

struct ScriptedParticlePool {
    struct Task task;
    void *sequenceData;
    struct ScriptedParticle particles[48];
};

struct ScriptedParticlePool *ScriptedParticle_EnsurePool(void);
struct ScriptedParticle *ScriptedParticle_AllocateSlot(struct ScriptedParticlePool *);

#endif
