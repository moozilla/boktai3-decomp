#ifndef GUARD_CONTACT_IMPACT_EFFECT_H
#define GUARD_CONTACT_IMPACT_EFFECT_H
#include "engine.h"
#include "sun_orb_effect.h"
#include "scene_event_dispatch.h"
#include "orbit_motion.h"
struct ContactImpactEffect {
    struct Task task;
    u8 animation[0x1C];
    struct OrbitEffectSprite sprite;
    struct SceneEventReceiver receiver;
    u8 receiverState[0xC];
    u16 rampFrom, rampTo, coordinateMode, shakeThreshold;
    s16 blinkTimer, blinkState;
    struct OrbitMotion motion;
    u32 stateResult;
    struct ContactPosition world, target;
    u32 moveTimer;
    s32 name;
    void (*update)(struct ContactImpactEffect *);
    s32 timer;
};
struct ContactImpactPackedPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
u32 RegistryF3E4_GetStateResult(struct ContactImpactEffect *);
void ContactImpactEffect_BeginMotion(struct ContactImpactEffect *, struct ContactPosition *,
                                     struct ContactPosition *, u32, u32, u32, s32, u32);
void SceneTarget_BeginMove(struct ContactImpactEffect *, struct ContactPosition *,
                           struct ContactPosition *, s32, u32, u32, u32);
void ContactImpactEffect_BeginTimedPosition(struct ContactImpactEffect *, struct ContactPosition *,
                                            u32);
void RegistryF3E4_ClearState(struct ContactImpactEffect *);
void ContactImpactEffect_BeginInterpolatedPosition(struct ContactImpactEffect *,
                                                   struct ContactPosition *,
                                                   struct ContactPosition *, s32, u32, u32);
void ContactImpactEffect_UpdateBlinkFrame(struct ContactImpactEffect *);
void RegistryF3E4_StartAppearing(struct ContactImpactEffect *);
void RegistryF3E4_StartDisappearing(struct ContactImpactEffect *);
void RegistryF3E4_SetHidden(struct ContactImpactEffect *);
void SpriteController_UpdateStopped(struct ContactImpactEffect *);
void SpriteController_AnimateGrowing(struct ContactImpactEffect *);
void SpriteController_AnimateShrinking(struct ContactImpactEffect *);
void RegistryF3E4_UpdateVisibleIdle(struct ContactImpactEffect *);
void ContactImpactEffect_UpdateShakingPosition(struct ContactImpactEffect *);
void ContactImpactEffect_UpdateInterpolatedPosition(struct ContactImpactEffect *);
void ContactImpactEffect_UpdateContact(struct ContactImpactEffect *);
void ContactImpactEffect_OnUnusedHook(void);
void ContactImpactEffect_ProcessMessages(struct ContactImpactEffect *);
void RegistryF3E4_OnTaskUpdate(void *);
void RegistryF3E4_OnTaskDestroy(void *);
struct ContactImpactEffect *RegistryF3E4_CreateHidden(s32, s32);
void RegistryF3E4_Show(void);
void RegistryF3E4_Hide(void);
struct ContactImpactEffect *RegistryF3E4_Create(u32);
#endif
