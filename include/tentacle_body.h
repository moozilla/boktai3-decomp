#ifndef GUARD_TENTACLE_BODY_H
#define GUARD_TENTACLE_BODY_H
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_availability.h"
#include "scene_actor_chase_reaction.h"
#include "tentacle_spread.h"
#include "actor_animation_controller.h"
#include "engine.h"
/* Complete allocation used by TentacleEnemy_Create; private roles are tentative. */
struct TentacleBody {
    void *object;
    void *drawable;
    u8 drawableKind;
    u8 unknown9[0x37];
    u8 movementProbe[0x10];
    struct SceneActorBody body;
    u8 category;
    u8 unknown99;
    u16 field9A;
    s16 health;
    u8 field9E, field9F, fieldA0, fieldA1;
    u16 fieldA2, fieldA4;
    s16 attachmentLimit; /* +0xA6: signed attachment restart threshold. */
    s16 fieldA8;
    u16 fieldAA;
    u32 fieldAC;
    s16 defaultHealth;
    u16 fieldB2;
    struct SceneActorAttackParameters attacks;
    struct SceneActorChaseTracking tracking;
    u8 unknown100[4];
    u32 word104;
    u32 word108, word10C, word110;
    u32 flags114;
    struct SceneActorDefaultControl saved;
    struct ContactCollider colliderA;
    struct ContactCollider colliderB;
    u16 alternatePalette; /* +0x1DC: selected by status bit 0x4000. */
    u16 paletteBank;
    u16 basePaletteBank;
    u16 paletteAlternateBank;
    u16 alternateOffset; /* +0x1E4: alternate palette offset. */
    u16 paletteOffset;
    u16 field1E8;
    u16 field1EA;
    u32 field1EC, field1F0;
    u8 unknown1F4[0x28];
    void *target;
    void *field220;
    void *entryTarget224;
    void *entryTarget228;
    void *savedTarget;
    void *reactionTarget230;
    void *deathControl; /* +0x234: controller installed by defeat dispatch. */
    void *field238, *field23C;
    void *field240;
    u8 unknown244[4];
    void *callback248;
    void *messageCallback;
    union SceneActorCommandTable commandTable;
    u8 unknown254[4];
    void *updateCallback, *destroyCallback;
    union SceneActorAnimationQuerySlot animationQuery;
    void *damageCallback, *contactCallback;
    void *update26C;
    union SceneActorContactQuerySlot contactQuery;
    void *contextCallback, *defaultReaction;
    void **callbacks27C, **callbacks280, **callbacks284;
    void **alternateBehaviorCallbacks; /* +0x288: indexed by behavior2AA. */
    void **behaviorCallbacks;          /* +0x28C: indexed by behavior2AA. */
    void **callbacks290, **callbacks294;
    void **phaseCallbacks; /* +0x298: indexed by phase2A9. */
    void **callbacks29C, **callbacks2A0;
    u8 unknown2A4[4];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0, field2B1, field2B2, field2B3;
    u8 enabled, field2B5, field2B6, animationIndex;
    u16 field2B8;
    u8 unknown2BA[2];
    u16 defeatTimer;
    u16 groundHeight2BE;
    u16 flags2C0, field2C2;
    u32 profileMode;
    u16 field2C8;
    u16 field2CA;
    u32 flags2CC;
    u32 flags2D0;
    u16 flags2D4;
    u16 flags2D6;
    u32 field2D8, field2DC, field2E0;
    u16 reactionSoundVariant;
    u16 field2E6;
    u16 field2E8;
    u16 field2EA;
    struct ContactPosition secondaryMotion;
    u16 terrainCell;
    u16 previousTerrainCell;
    u16 savedTargetCell;
    u16 regionId;
    struct ContactPosition previousPosition;
    u32 field304;
    u32 profileId;
    u32 field30C;
    struct ContactPosition reactionStart,
        reactionEnd; /* +0x310/+0x318: reaction position snapshots. */
    u8 shadow[0xAC]; /* +0x320: first byte gates the shadow enable/disable API. */
    u8 field3CC, unknown3CD[3];
    void *context;
    struct TentacleAnimationSegment segments[4];
    u8 unknown874, spin;
    u8 unknown876[0x5A];
    u16 attackFlags;
    u8 unknown8D2[8];
    s16 segmentCount;
    u8 unknown8DC[10];
    u16 releaseFrame;
    u8 unknown8E8[2];
    u8 spreadStage;
    u8 unknown8EB[0x934 - 0x8EB];
};
struct TentacleBodyDrawable {
    struct ActorSprite sprite;
    u8 unknown24[8];
    u8 animation[0x1C];
    struct DisplaySequenceState sequence;
    void *resource;
};

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);
void TentacleActor_EnterHitReaction(struct SceneActorRegistryResetView *);
void TentacleBody_GetDirectionFrame(u8, u32 *, u32 *);
void TentacleBody_ResetDirectionalAnimations(void *, s32);
void TentacleBody_RefreshDirectionalAnimations(void *, s32);
void TentacleActor_AssignSegmentPhases(void *);
void ActorTrail_RestoreSavedState(struct SceneActorRegistryResetView *s);
void ActorTrail_ResetAtSavedPosition(struct SceneActorRegistryResetView *);
void SceneActorPalette_ClearCompletedTransitions(void);
void TentacleBody_RestartSegmentAnimations(void *, u32, u32, u32);
void TentacleBody_UpdateSpreadRelease(struct SceneActorRegistryResetView *actor, s32 frame);
void TentacleBody_UpdateChargeSpread(struct SceneActorRegistryResetView *actor, s32 frame);
void ActorTrail_ApplyAngularOffsets(void *, s32, s32, s32);
void TentacleBody_BeginDirectedAttack(struct SceneActorRegistryResetView *actor);
void TentacleBody_UpdateAttackRecovery(struct SceneActorRegistryResetView *actor, s32 frame);
void SceneActor_AimArcAtPlayer(void *);
void TentacleBody_OnAttackCollision(u8 *unused, struct ContactCollider *obj,
                                    struct SceneActorRegistryResetView *ctx);
void TentacleBody_OnBodyContact(struct SceneActorBehaviorContactView *source,
                                struct SceneActorBehaviorContactView *destination,
                                struct SceneActorRegistryResetView *actor);
void TentacleBody_UpdateAttackWindup(struct SceneActorAnimationView *actor);
void TentacleBody_ResetDirectionalPose(struct SceneActorAnimationView *actor);
void TentacleBody_OnIdleState(void);
void TentacleBody_UpdateInactiveState(void);
void TentacleBody_UpdateTimedAction(struct SceneActorRegistryResetView *p, s32 time);
void TentacleBody_UpdateHiddenTransition(struct SceneActorRegistryResetView *s, s32 arg,
                                         u32 unused);
void TentacleBody_UpdateAnimationEleven(struct SceneActorRegistryResetView *p);
void TentacleBody_UpdateSpreadWindup(struct SceneActorRegistryResetView *actor);
void TentacleBody_UpdateSpreadPreparation(struct SceneActorRegistryResetView *actor);

static inline void TentacleBody_SelectAnimation(struct SceneActorAnimationView *actor,
                                                u32 selection, u32 variant, u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        selection += variant;
        if (!force && actor->selection == selection && !actor->restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        actor->selection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            flags &= 1;
            if (flags)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    actor->restart = 0;
}
#endif
