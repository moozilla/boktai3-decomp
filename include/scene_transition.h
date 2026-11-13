#ifndef GUARD_SCENE_TRANSITION_H
#define GUARD_SCENE_TRANSITION_H
#include "engine.h"
#include "orbit_motion.h"
#include "contact_swarm.h"
struct ContactImpactEffect;
struct SceneTransition {
    struct Task task;
    void *background;
    struct ContactPosition scroll;
    struct OrbitMotion motion;
    s32 exitScript;
    struct ContactImpactEffect *target;
    void (*update)(struct SceneTransition *);
    s32 timer;
    u32 particleFlags;
};
void SceneTransition_SpawnParticleBurst(s32, s32, s32);
void SceneTransition_SpawnFlyingParticle(s32, s32, u32);
void Registry6A37_UpdateBackgroundScroll(struct SceneTransition *);
void SceneTransition_BeginOffsetMove(struct SceneTransition *);
void SceneTransition_BeginVectorTween(struct SceneTransition *);
void SceneTransition_BeginFirstScroll(struct SceneTransition *);
void Registry6A37_StartSoundPhase(struct SceneTransition *);
void Registry6A37_StartFadeIn(struct SceneTransition *);
void Registry6A37_StartFadeOut(struct SceneTransition *);
void Registry6A37_UpdateDisplayHold(struct SceneTransition *);
void Registry6A37_UpdateSecondScrollTransition(struct SceneTransition *);
void Registry6A37_UpdateFirstScrollTransition(struct SceneTransition *);
void SceneTransition_UpdateShakingBackground(struct SceneTransition *);
void Registry6A37_UpdateFadeOut(struct SceneTransition *);
void Registry6A37_UpdateFadeIn(struct SceneTransition *);
void Registry6A37_Update(struct SceneTransition *);
void SceneTransition_LoadBackground(struct SceneTransition *);
void Registry6A37_OnDestroy(void *);
void *SceneTransition_StartSunScript(void);
s32 SceneTransition_Initialize(struct SceneTransition *, void *);
void *Registry6A37_Create(void *);
#endif
