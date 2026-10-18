#include "global.h"
#define SceneEffect_InitializeEmitterOptions sub_08199374

struct EmitterRingSlot { u32 update, destroy; u8 data[0x150]; };
struct EmitterParticleSlot { u32 update, destroy; u8 data[0xb0]; };
struct SceneEffectEmitter {
    u8 pad00[0x1c];
    u8 state, timer, ringCount, particleCount;
    u16 burstCount, burstInterval, ringCountLimit, ringInterval, speed, lifetime;
    u8 pad2C[8];
    struct EmitterRingSlot rings[1];
    struct EmitterParticleSlot particles[4];
};
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);

s32 sub_08199374(struct SceneEffectEmitter *emitter)
{
    struct EmitterRingSlot *ring;
    struct EmitterParticleSlot *particle;
    s32 i;
    u32 zero;
    u32 stride;
    emitter->state = 0;
    emitter->timer = 0;
    emitter->ringCount = 0;
    emitter->particleCount = 0;
    emitter->burstCount = 3;
    emitter->burstInterval = 1;
    emitter->ringCountLimit = 5;
    emitter->ringInterval = 3;
    emitter->speed = 10;
    emitter->lifetime = 10;
    if (Script_SeekToKeyword('b')) {
        emitter->burstCount = Script_GetValue();
        emitter->burstInterval = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        emitter->ringCountLimit = Script_GetValue();
        emitter->ringInterval = Script_GetValue();
    }
    if (Script_SeekToKeyword('s')) {
        emitter->speed = Script_GetValue();
        emitter->lifetime = Script_GetValue();
    }
    zero = 0;
    ring = emitter->rings;
    stride = sizeof(*ring);
    i = 0;
    do {
        ring->update = zero;
        ring->destroy = zero;
        ring = (struct EmitterRingSlot *)((u8 *)ring + stride);
    } while (--i >= 0);
    zero = 0;
    particle = emitter->particles;
    i = 3;
    do {
        particle->update = zero;
        particle->destroy = zero;
        particle++;
    } while (--i >= 0);
    return 0;
}
