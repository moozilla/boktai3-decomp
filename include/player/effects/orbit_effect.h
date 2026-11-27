#ifndef GUARD_PLAYER_ORBIT_EFFECT_H
#define GUARD_PLAYER_ORBIT_EFFECT_H
#include "player/player.h"
#include "engine/engine.h"
#include "scene/collision/contact_collider.h"

struct PlayerOrbitParticleMotion {
    u8 active, angle, riseSpeed, expandDelay, animationPhase, angularSpeed, lifetime, state;
    u16 radius, unknown32;
    u16 velocityX, velocityY, velocityZ, unknown3A; /* wrapping coordinate increments */
};
struct PlayerOrbitParticle {
    u32 flags;
    u8 unknown4[0xB], priority;
    u8 attribute, unknown11[7];
    struct ContactPosition position;
    u8 unknown20[8];
    struct PlayerOrbitParticleMotion motion;
};
struct PlayerOrbitEffect {
    struct Task task;
    struct Player *owner;
    struct AnimSpriteResource animationData;
    struct AnimSprite drawable;
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
void PlayerOrbitEffect_SpawnParticle(struct PlayerOrbitEffect *, struct ContactPosition *, s32);
void PlayerOrbitEffect_UpdateStatTransfer(struct PlayerOrbitEffect *);
void PlayerTransition_StepReveal(struct PlayerOrbitEffect *);
#endif
