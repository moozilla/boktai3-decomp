#ifndef GUARD_SCENE_COMBAT_ACTOR_H
#define GUARD_SCENE_COMBAT_ACTOR_H
#include "contact_collider.h"
#include "armor_palette_controller.h"

struct ParticleVector {
    s32 x : 16, y : 16, z : 16;
    u32 : 16;
};
union ParticleVectorStorage {
    struct ParticleVector packed;
    u16 components[4];
    s16 signedComponents[4];
};
union SceneCombatPosition {
    struct { u16 x, floor, z, padding; } values;
    u16 components[4];
};
struct BouncingParticleSprite {
    u32 flags;
    u8 unknown4[2], phase, unknown7[9];
    u16 frame;
    u8 unknown12[0xA];
    union ParticleVectorStorage position;
    u8 unknown24[8];
};
struct BouncingSceneParticle {
    void (*update)(struct BouncingSceneParticle *), (*destroy)(struct BouncingSceneParticle *);
    struct BouncingParticleSprite sprite;
    union ParticleVectorStorage velocity;
    s16 floor;
    u8 bounced;
};
struct ParticleAnimation {
    u8 unknown0[6];
    u16 paletteId;
    u8 unknown8[4];
    void *palette;
    u8 unknown10[0xC];
};
struct SceneCombatSnapshot {
    u8 phase, mode;
    u16 counter, timer, value6, value8, valueA, savedAttack, savedDefense;
};
struct SceneCombatState {
    struct SceneCombatSnapshot common;
    u16 value10, value12;
};
struct ActorControllerNavigation {
    u16 probe[6];
    struct ContactPosition position;
    u8 unknown14[0x1C];
    struct ContactPosition referencePosition;
};
/* One actor owns two loose particles, a facing marker, and nine fragments. */
struct SceneCombatActor {
    u8 unknown0[0x18];
    struct ActorControllerNavigation navigation;
    u8 unknown50[0x84];
    union SceneCombatPosition position;
    u8 unknownDC[0x12];
    u16 paletteId;
    u8 unknownF0[0xC];
    void *palette;
    u8 unknown100[0x90];
    struct ContactCollider colliders[3];
    u8 unknown28C[0x54];
    struct ParticleAnimation fragmentAnimations[2];
    struct ParticleAnimation particleAnimation, markerAnimation;
    struct BouncingSceneParticle looseParticles[2];
    struct BouncingSceneParticle facingMarker;
    struct BouncingSceneParticle fragments[9];
    struct ArmorPaletteController paletteController;
    u8 unknownADC[0x4A];
    u16 attack, defense;
    u8 unknownB2A[8];
    u16 flags;
    u8 unknownB34[4];
    u8 kind, heading;
    u8 targetHeading, unknownB3B;
    u8 behavior, behaviorPhase, behaviorCounter;
    u8 action, actionPhase, actionReady, actionCounter, animationComplete;
    u8 unknownB44[0x5DC];
    struct SceneCombatState state;
};

s32 Callback_InvokeActor(void *, void *);
void Sprite_UnlinkFromList(void *, u32);
void Sprite_Init(void *, void *, s32);
void SceneParticle_UpdateBounce(struct BouncingSceneParticle *);
void BouncingEffect_RemoveSprite(struct BouncingSceneParticle *);
void ActorOverlay_InitSprite(struct SceneCombatActor *);
void SceneCombatActor_InitBouncingParticle(struct SceneCombatActor *);
extern const u8 gUnk_0824E6A4[], gUnk_0824E6B4[], gUnk_0824E6C4[];
extern const u8 gUnk_0824E6D4[], gUnk_0824E6E6[];
#endif
