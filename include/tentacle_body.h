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
#include "room_actor.h"
/* Complete allocation used by TentacleEnemy_Create; private roles are tentative. */
struct TentacleBody {
    struct RoomActor base;
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
void TentacleActor_EnterHitReaction(struct ChaseActor *);
void TentacleBody_GetDirectionFrame(u8, u32 *, u32 *);
void TentacleBody_ResetDirectionalAnimations(void *, s32);
void TentacleBody_RefreshDirectionalAnimations(void *, s32);
void TentacleActor_AssignSegmentPhases(void *);
void ActorTrail_RestoreSavedState(struct ChaseActor *s);
void ActorTrail_ResetAtSavedPosition(struct ChaseActor *);
void SceneActorPalette_ClearCompletedTransitions(void);
void TentacleBody_RestartSegmentAnimations(void *, u32, u32, u32);
void TentacleBody_UpdateSpreadRelease(struct ChaseActor *actor, s32 frame);
void TentacleBody_UpdateChargeSpread(struct ChaseActor *actor, s32 frame);
void ActorTrail_ApplyAngularOffsets(void *, s32, s32, s32);
void TentacleBody_BeginDirectedAttack(struct ChaseActor *actor);
void TentacleBody_UpdateAttackRecovery(struct ChaseActor *actor, s32 frame);
void SceneActor_AimArcAtPlayer(void *);
void TentacleBody_OnAttackCollision(u8 *unused, struct ContactCollider *obj,
                                    struct ChaseActor *ctx);
void TentacleBody_OnBodyContact(struct SceneActorBehaviorContactView *source,
                                struct SceneActorBehaviorContactView *destination,
                                struct ChaseActor *actor);
void TentacleBody_UpdateAttackWindup(struct RoomActor *actor);
void TentacleBody_ResetDirectionalPose(struct RoomActor *actor);
void TentacleBody_OnIdleState(void);
void TentacleBody_UpdateInactiveState(void);
void TentacleBody_UpdateTimedAction(struct ChaseActor *p, s32 time);
void TentacleBody_UpdateHiddenTransition(struct ChaseActor *s, s32 arg, u32 unused);
void TentacleBody_UpdateAnimationEleven(struct ChaseActor *p);
void TentacleBody_UpdateSpreadWindup(struct ChaseActor *actor);
void TentacleBody_UpdateSpreadPreparation(struct ChaseActor *actor);

static inline void TentacleBody_SelectAnimation(struct RoomActor *actor, u32 selection, u32 variant,
                                                u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->animationIndex = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        selection += variant;
        if (!force && actor->animationIndex == selection && !actor->restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        actor->animationIndex = selection;
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
