#ifndef GUARD_PARTICLE_BURST_EFFECT_H
#define GUARD_PARTICLE_BURST_EFFECT_H
#include "particle_effect_state.h"
#include "player_cue_sequence.h"
#include "scene_arc.h"

struct ParticleBurstManager {
    u8 task[0x18];
    struct BurstEntry entries[6];
    u32 frame;
};
struct ParticleBurstParameters {
    u16 unknown0, duration, unknown4, value;
};
struct ParticleBurstBoundsHalf {
    u32 low : 16;
    s32 high : 16;
};
union ParticleBurstBounds {
    struct {
        struct ParticleBurstBoundsHalf horizontal, vertical;
    } axis;
    u32 words[2];
};
void ParticleBurstEffect_SetStateCallback(struct BurstEntry *, void (*)(struct BurstEntry *));
void SpriteEffect_HideWithScatterBurst(struct BurstEntry *);
void ParticleBurstEffect_TickActive(struct BurstEntry *);
void ParticleBurstEffect_UpdateFalling(struct BurstEntry *);
s32 CallbackPool_Update(struct ParticleBurstManager *);
s32 ParticleBurstEffect_DestroyManager(void);
s32 SceneProcParticles_Initialize(struct ParticleBurstManager *);
struct ParticleBurstManager *ParticleBurstEffect_EnsureManager(void);
s32 CallbackPool_MarkNextOfKind(struct ParticleBurstManager *, s32, s32, u8 *);
u8 SceneProcParticles_TestCapacity(struct ParticleBurstManager *, u32, s32);
void ParticleBurstEffect_OnAnimationComplete(struct BurstEntry *);
void SceneProcParticles_SubmitBounds(struct BurstEntry *, void (*)(struct BurstEntry *), u32, u32);
s32 SpritePool_StartEntry(struct BurstPos *, s32, u32, struct ParticleBurstParameters *, u32);
void CallbackPool_RemoveEntry(u32);
#endif
