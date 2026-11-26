#ifndef GUARD_EFFECTS_SCREEN_MAGIC_CANNON_DEPARTURE_H
#define GUARD_EFFECTS_SCREEN_MAGIC_CANNON_DEPARTURE_H
#include "engine/engine.h"
#include "objects/dungeon_prop/orbit_motion.h"
#include "scene/collision/contact_swarm.h"
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
void MagicCannonDeparture_UpdateBackgroundScroll(struct SceneTransition *);
void SceneTransition_BeginOffsetMove(struct SceneTransition *);
void SceneTransition_BeginVectorTween(struct SceneTransition *);
void SceneTransition_BeginFirstScroll(struct SceneTransition *);
void MagicCannonDeparture_StartSoundPhase(struct SceneTransition *);
void MagicCannonDeparture_StartFadeIn(struct SceneTransition *);
void MagicCannonDeparture_StartFadeOut(struct SceneTransition *);
void MagicCannonDeparture_UpdateDisplayHold(struct SceneTransition *);
void MagicCannonDeparture_UpdateSecondScrollTransition(struct SceneTransition *);
void MagicCannonDeparture_UpdateFirstScrollTransition(struct SceneTransition *);
void SceneTransition_UpdateShakingBackground(struct SceneTransition *);
void MagicCannonDeparture_UpdateFadeOut(struct SceneTransition *);
void MagicCannonDeparture_UpdateFadeIn(struct SceneTransition *);
void MagicCannonDeparture_Update(struct SceneTransition *);
void SceneTransition_LoadBackground(struct SceneTransition *);
void MagicCannonDeparture_OnDestroy(void *);
void *SceneTransition_StartSunScript(void);
s32 SceneTransition_Initialize(struct SceneTransition *, void *);
void *MagicCannonDeparture_Create(void *);
#endif
