#ifndef GUARD_SCENE_GRAPHICS_ACTOR_H
#define GUARD_SCENE_GRAPHICS_ACTOR_H

#include "global.h"
#include "engine/video/anim_sprite.h"
#include "scene/actor/scene_actor_components.h"
#include "scene/collision/contact_collider.h"

struct ContactActorPool;
struct ContactSwarm;
struct BackgroundFlashController;
struct Player;

/* 08021EA8-08024F2C: drawable setup, targeting, resource/health costs and the
 * recovery/action dispatch pipeline for struct StateActor (see state_actor.h
 * for the 0x90-0x2CC fields this range pinned down). This header names
 * `groundEffect`'s contents as far as this unit's evidence goes; its full
 * extent is still unknown past 0x9E0. Field names are provisional
 * (confidence=low). */
struct SceneActorGraphics {
    s16 scale, scaleMin, scaleMax, scaleStep; /* 0x344: SceneActorGraphics_OscillateScale. */
    u16 paletteFlashFrames; /* 0x34C: set when a hit is blocked. */
    u16 hitImmunityFrames;  /* 0x34E: copied from the attacker; damage is ignored while set. */
    u8 unk350[2];
    s16 recoveryGroundHeight; /* 0x352: compared with the forward terrain probe. */
    u16 approachFrames; /* 0x354: duration passed to the polar interpolator. */
    u16 recoveryTimer;  /* 0x356: frames of recovery set by the *ThenRecover handlers. */
    s32 stepTableKind;  /* 0x358: selects one of the gUnk_08604DC4 family tables. */
    s16 attackWidth0, attackHeight0, attackDepth0; /* 0x35C: variant-0 attack properties. */
    s16 attackWidth1, attackHeight1, attackDepth1; /* 0x362: variant-1 attack properties. */
    s16 grabHealthAmount; /* 0x368: taken from the grabbed player (tentative). */
    s16 impactHazardResource; /* 0x36A: passed to AnimatedHazard_Create. */
    s16 chargeShotPower;
    u8 unk36E[0x374 - 0x36E];
    s16 flashWindupFrames; /* 0x374 */
    u8 unk376[0x378 - 0x376];
    s16 routeProjectileAmount, routeProjectileKind;
    s16 travelColliderA, travelColliderB;
    s16 contactStatHundredths; /* 0x380 */
    u8 unk382;
    u8 trackedPlayerEscaped; /* 0x383: set after enough facing changes in the grab. */
    u8 barrierCount; /* 0x384 */
    u8 retreatFrames; /* 0x385: frames of the dying actor's slide back from blocked or low ground. */
    u8 unk386[2];
    u32 reactionScript; /* 0x388: called after the special hit reaction. */
    u8 effectScale; /* 0x38C: SceneActorGraphics_SelectResource / SceneActors_DispatchCommands. */
    u8 unk38D;
    u8 effectMode;     /* 0x38E */
    u8 effectDuration; /* 0x38F */
    u32 resourceId;    /* 0x390 */
    u8 particleFlag;   /* 0x394: cleared by SceneActorState_AdvanceToState06. */
    s8 trackedPlayer; /* 0x395: player slot selected by the contact box. */
    u8 grabStartFacing; /* 0x396: the grabbed player's facing cue when seized. */
    u8 grabLastFacing;
    u8 shakeAngle; /* 0x398: the struggle direction, or random during a flash windup. */
    u8 shakeStrength;
    u8 grabFacingChanges; /* 0x39A */
    u8 reactionDelay; /* 0x39B: set by the anchored and death reactions. */
    s32 resourceCostAccumulator; /* 0x39C: SceneActors_AddResourceCost. */
    s32 healthCostAccumulator;   /* 0x3A0: SceneActors_AddHealthCost. */
    s32 healthCostBaseline;      /* 0x3A4: snapshot of health at contact-pool start. */
    u32 unk3A8; /* cleared when a flash windup starts. */
    u32 animationFlags; /* 0x3AC: bit 0 selects alternate animation sequences. */
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
    struct ContactPosition snapPosition; /* 0x9C4: set by scene command 12. */
    struct ContactPosition grabPosition; /* 0x9CC: the grabbed player's position. */
    struct ContactSwarm *batPool; /* 0x9D4: bats spawned by the charge shot. */
    struct ContactActorPool *contactPool; /* 0x9D8: SceneActors_RequestContactPoolClear. */
    struct BackgroundFlashController *backgroundFlash; /* 0x9DC: SceneActors_StopBackgroundFlash. */
};

/* Actor-relative view used by contact, animation and palette handlers. */
/* A scene command as the actor reads it: the event header and up to three halfword arguments. */
struct SceneGraphicsCommand {
    u8 unk0[6];
    u8 kind, argumentCount;
    s16 arguments[3];
};

struct SceneGraphicsActor {
    u8 unk0[0x48];
    struct SceneActorBody body; /* 0x48 */
    u8 unk90[0x94 - 0x90];
    s16 health; /* 0x94 */
    u8 unk96[0xBC - 0x96];
    u32 hitStatRewardFlags; /* 0xBC: copied from the collider that last hit the actor. */
    u32 hitReactionFlags;   /* 0xC0 */
    u8 unkC4[0xC8 - 0xC4];
    s32 playerDistances[1]; /* 0xC8: squared distance to each player, from playerPositions. */
    u8 playerBearings[1];   /* 0xCC */
    u8 unkCD[0xE2 - 0xCD];
    u8 contactHundredths; /* 0xE2 */
    u8 unkE3[0x10C - 0xE3];
    /* 0x10C: setup stores each player and then its position one slot later, so only
     * entry zero stays paired. */
    struct Player *players[1];
    struct ContactPosition *playerPositions[4]; /* 0x110 */
    u8 unk120[0x174 - 0x120];
    u8 playerCount; /* 0x174 */
    u8 unk175[0x1CC - 0x175];
    struct ContactCollider overlayCollider; /* 0x1CC */
    struct AnimSpriteResource animationResource; /* 0x220: keyframe resource. */
    struct AnimSprite animation; /* 0x240: body animation playback. */
    u8 unk2A0[0x30C - 0x2A0];
    u8 approachMotion[0x32C - 0x30C]; /* 0x30C: polar interpolation state of the charge approach. */
    struct ContactPosition retreatTarget; /* 0x32C: the dying actor slides back toward this. */
    u8 unk334[0x33C - 0x334];
    s32 pendingDamage;
    u8 damagePending;
    u8 specialReaction; /* 0x341: sound, sparks and script on reaction completion. */
    u8 hitAngle, variant;
    struct SceneActorGraphics graphics; /* 0x344 */
};

static inline void SceneGraphicsActor_ClearPendingDamage(struct SceneGraphicsActor *actor)
{
    actor->pendingDamage = 0;
    actor->damagePending = 0;
}

/* Choose the palette resource and its flash behavior: mode 2 scales the effect by its
 * duration, mode 1 starts it from zero. */
static inline void SceneActorGraphics_SetEffect(struct SceneGraphicsActor *actor, int mode,
                                                int resource, int duration)
{
    actor->graphics.resourceId = resource;
    actor->graphics.effectMode = mode;
    actor->graphics.effectDuration = duration;
    if (actor->graphics.effectMode == 2) {
        u32 scale = duration == 5 ? 32 : 64;
        actor->graphics.effectScale = scale;
    } else if (actor->graphics.effectMode == 1)
        actor->graphics.effectScale = 0;
}

#endif
