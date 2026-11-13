#ifndef GUARD_SCENE_COMBAT_ACTOR_H
#define GUARD_SCENE_COMBAT_ACTOR_H
#include "contact_collider.h"

struct ParticleVector {
    s32 x : 16, y : 16, z : 16;
    u32 : 16;
};
struct BouncingParticleSprite {
    u32 flags;
    u8 unknown4[2], phase, unknown7[9];
    u16 frame;
    u8 unknown12[0xA];
    struct ParticleVector position;
    u8 unknown24[8];
};
struct BouncingSceneParticle {
    void (*update)(struct BouncingSceneParticle *), (*destroy)(struct BouncingSceneParticle *);
    struct BouncingParticleSprite sprite;
    struct ParticleVector velocity;
    s16 floor;
    u8 bounced;
};
struct ParticleAnimation {
    u8 data[0x1C];
};
struct SceneCombatSnapshot {
    u8 phase, mode;
    u16 counter, timer, value6, value8, valueA, savedAttack, savedDefense;
};
struct SceneCombatState {
    struct SceneCombatSnapshot common;
    u16 value10, value12;
};
/* One actor owns two loose particles, a facing marker, and nine fragments. */
struct SceneCombatActor {
    u8 unknown0[0xD4];
    u16 x, floor, z;
    u8 unknownDA[0xB6];
    struct ContactCollider colliders[3];
    u8 unknown28C[0x54];
    struct ParticleAnimation fragmentAnimations[2];
    struct ParticleAnimation particleAnimation, markerAnimation;
    struct BouncingSceneParticle looseParticles[2];
    struct BouncingSceneParticle facingMarker;
    struct BouncingSceneParticle fragments[9];
    u8 unknown650[0x4D6];
    u16 attack, defense;
    u8 unknownB2A[8];
    u16 flags;
    u8 unknownB34[4];
    u8 kind, heading;
    u8 unknownB3A[0x5E6];
    struct SceneCombatState state;
};

s32 Callback_InvokeActor(void *, void *);
void Sprite_UnlinkFromList(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneParticle_UpdateBounce(struct BouncingSceneParticle *);
void BouncingEffect_RemoveSprite(struct BouncingSceneParticle *);
void ActorOverlay_InitSprite(struct SceneCombatActor *);
void SceneCombatActor_InitBouncingParticle(struct SceneCombatActor *);
extern const u8 gUnk_0824E6A4[], gUnk_0824E6B4[], gUnk_0824E6C4[];
extern const u8 gUnk_0824E6D4[], gUnk_0824E6E6[];
#endif
