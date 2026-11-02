#ifndef GUARD_SCENE_GRAPHICS_ACTOR_H
#define GUARD_SCENE_GRAPHICS_ACTOR_H

#include "global.h"

struct ContactActorPool;
struct BackgroundFlashController;

/* 08021EA8-08024F2C: drawable setup, targeting, resource/health costs and the
 * recovery/action dispatch pipeline for struct StateActor (see state_actor.h
 * for the 0x90-0x2CC fields this range pinned down). This header names
 * `groundEffect`'s contents as far as this unit's evidence goes; its full
 * extent is still unknown past 0x9E0. Field names are provisional
 * (confidence=low). */
struct SceneActorGraphics {
    s16 scale, scaleMin, scaleMax, scaleStep; /* 0x344: SceneActorGraphics_OscillateScale. */
    u8 unk34C[0x354 - 0x34C];
    u16 approachFrames; /* 0x354: duration passed to the polar interpolator. */
    u16 recoveryTimer;  /* 0x356: frames of recovery set by the *ThenRecover handlers. */
    s32 stepTableKind;  /* 0x358: selects one of the gUnk_08604DC4 family tables. */
    s16 attackWidth0, attackHeight0, attackDepth0; /* 0x35C: variant-0 attack properties. */
    s16 attackWidth1, attackHeight1, attackDepth1; /* 0x362: variant-1 attack properties. */
    u8 unk368[4];
    s16 chargeShotPower;
    u8 unk36E[0x378 - 0x36E];
    s16 routeProjectileAmount, routeProjectileKind;
    s16 travelColliderA, travelColliderB;
    u8 unk380[0x38C - 0x380];
    u8 effectScale; /* 0x38C: SceneActorGraphics_SelectResource / SceneActors_DispatchCommands. */
    u8 unk38D;
    u8 effectMode;     /* 0x38E */
    u8 effectDuration; /* 0x38F */
    u32 resourceId;    /* 0x390 */
    u8 particleFlag;   /* 0x394: cleared by SceneActorState_AdvanceToState06. */
    u8 unk395[0x39C - 0x395];
    s32 resourceCostAccumulator; /* 0x39C: SceneActors_AddResourceCost. */
    s32 healthCostAccumulator;   /* 0x3A0: SceneActors_AddHealthCost. */
    s32 healthCostBaseline;      /* 0x3A4: snapshot of health at contact-pool start. */
    u8 unk3A8[0x3B0 - 0x3A8];
    u8 stepTableRow;                     /* 0x3B0 */
    u8 stepTableCol;                     /* 0x3B1 */
    u16 stepTableValue;                  /* 0x3B2 */
    u16 stepTableValuePrev;              /* 0x3B4 */
    u8 variantToggle, variantTogglePrev; /* 0x3B6: SceneActorGraphics_UpdateVariant. */
    u8 unk3B8, flashPhase, projectileDirection, unk3BB;
    u32 initialFlashFlag;
    u32 backgroundFlashFlag; /* 0x3C0: SceneActors_StopBackgroundFlash. */
    u8 paletteBank0[0x200];  /* 0x3C4 */
    u8 paletteBank1[0x200];  /* 0x5C4 */
    u8 paletteBank2[0x200];  /* 0x7C4 */
    u8 unk9C4[0x9D8 - 0x9C4];
    struct ContactActorPool *contactPool; /* 0x9D8: SceneActors_RequestContactPoolClear. */
    struct BackgroundFlashController *backgroundFlash; /* 0x9DC: SceneActors_StopBackgroundFlash. */
};

/* Actor-relative view used by contact, animation and palette handlers. */
struct SceneGraphicsActor {
    u8 unk0[0x254];
    u16 animationField254;
    u8 unk256[0x33C - 0x256];
    s32 pendingDamage;
    u8 damagePending, unk341, hitAngle, variant;
    struct SceneActorGraphics graphics; /* 0x344 */
};

static inline void SceneGraphicsActor_ClearPendingDamage(struct SceneGraphicsActor *actor)
{
    actor->pendingDamage = 0;
    actor->damagePending = 0;
}

#endif
