#ifndef GUARD_PLAYER_ORBIT_EFFECT_H
#define GUARD_PLAYER_ORBIT_EFFECT_H
#include "player.h"
#include "engine.h"
#include "contact_collider.h"

struct PlayerOrbitResource {
    u32 words[8];
};

struct PlayerOrbitDrawable {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0xE];
    u8 state, unknown1B[5];
    struct SceneVec position;
    u8 unknown28[0x12];
    u16 attributes;
    u8 unknown3C[8];
    void *attachment;
    u8 unknown48[0x18];
};

struct PlayerOrbitParticleMotion {
    u8 active, angle, riseSpeed, expandDelay, animationPhase, angularSpeed, lifetime, state;
    u16 radius, unknown32;
    u16 velocityX, velocityY, velocityZ, unknown3A; /* wrapping coordinate increments */
};
struct PlayerOrbitParticle {
    u32 flags;
    u8 unknown4[0xB], priority;
    u8 attribute, unknown11[7];
    union PlayerEffectPosition position;
    u8 unknown20[8];
    struct PlayerOrbitParticleMotion motion;
};
struct PlayerOrbitEffect {
    struct Task task;
    struct Player *owner;
    struct PlayerOrbitResource animationData;
    struct PlayerOrbitDrawable drawable;
    struct ContactCollider collider;
    u16 unknownF0, phaseFrame, recoveryStep, targetStatA;
    void *resource;
    struct PlayerOrbitParticle particles[16];
    u16 particleFrame, unknown4BE;
    u32 unknown4C0;
    void (*updateState)(struct PlayerOrbitEffect *);
};

typedef char PlayerOrbitParticleSizeCheck[sizeof(struct PlayerOrbitParticle) == 0x3C ? 1 : -1];
typedef char PlayerOrbitEffectSizeCheck[sizeof(struct PlayerOrbitEffect) == 0x4C8 ? 1 : -1];

void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void PlayerBurst_SetStateCallback(struct PlayerOrbitEffect *, void (*)(struct PlayerOrbitEffect *));
void PlayerOrbitEffect_UpdateParticle(struct PlayerOrbitEffect *, struct PlayerOrbitParticle *);
void PlayerBurst_UpdateParticle(struct PlayerOrbitEffect *, struct PlayerOrbitParticle *);
void PlayerOrbitEffect_SpawnParticle(struct PlayerOrbitEffect *, struct SceneVec *, s32);
void PlayerOrbitEffect_UpdateStatTransfer(struct PlayerOrbitEffect *);
void PlayerTransition_StepReveal(struct PlayerOrbitEffect *);
#endif
