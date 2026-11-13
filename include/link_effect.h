#ifndef GUARD_LINK_EFFECT_H
#define GUARD_LINK_EFFECT_H
#include "engine.h"
#include "sun_orb_effect.h"
#include "contact_swarm_sprite.h"
#include "scene_projectile_params.h"
struct LinkEffect {
    struct Task task;
    void *projectileManager;
    struct OrbitEffectManager *manager;
    const struct OrbitEffectPosition *target;
    struct OrbitEffectPosition start, end;
    s8 orb, otherOrb;
    s16 collision;
    u16 timer;
    u8 state, phase, emitted, unknown3D[3];
};
struct LinkSpriteComponent {
    void (*update)(void *), (*destroy)(void *);
    struct ContactSwarmSprite sprite;
    u8 animation[0x26];
    u16 state;
    struct ContactCollider collider;
};
struct SceneOrbitHazard {
    struct Task task;
    struct LinkSpriteComponent effect;
    const struct OrbitEffectPosition *anchor;
    struct OrbitEffectPosition position, velocity;
    s16 effectParameter;
    u16 timer;
    u8 state, phase, angle, unknownE3;
};
void LinkEffect_UpdateEndpoints(struct LinkEffect *);
void SceneEffect_CreateLinkedEntry(struct LinkEffect *);
void LinkEffect_UpdateProjectileBurst(struct LinkEffect *);
void LinkEffect_UpdateSecondOrb(struct LinkEffect *);
void SceneBurstEmitter_SetState(struct LinkEffect *, u32);
void SceneBurstEmitter_TransitionIfComplete(struct LinkEffect *, u32);
s32 SceneEffect_UpdateEntryState(struct LinkEffect *);
u32 SceneBurstEmitter_OnDestroy(void);
s32 SceneBurstEmitter_Init(struct LinkEffect *);
struct LinkEffect *SceneEffect_CreateEntryTask(u32, const struct OrbitEffectPosition *);
void SceneBurstEmitter_RequestCleanup(struct LinkEffect *);
void DirectionalActor_GetForwardPosition(struct SceneOrbitHazard *, struct OrbitEffectPosition *);
void SceneOrbitHazard_SetState(struct SceneOrbitHazard *, s32);
void SceneOrbitHazard_TransitionIfComplete(struct SceneOrbitHazard *, u32);
void SceneOrbitHazard_UpdateOrbit(struct SceneOrbitHazard *);
void SceneOrbitHazard_UpdateTravel(struct SceneOrbitHazard *);
s32 SceneEffect_UpdateMovingState(struct SceneOrbitHazard *);
u32 SceneOrbitHazard_OnDestroy(struct SceneOrbitHazard *);
s32 SceneOrbitHazard_Init(struct SceneOrbitHazard *);
struct SceneOrbitHazard *SceneEffect_CreateMovingTask(u32, const struct OrbitEffectPosition *);
void SceneOrbitHazard_Activate(struct SceneOrbitHazard *);
#endif
