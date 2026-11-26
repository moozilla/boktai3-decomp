#ifndef GUARD_TENTACLE_BODY_H
#define GUARD_TENTACLE_BODY_H
#include "scene/actor/scene_actor_reset.h"
#include "npc/manager/scene_actor_animation.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/motion/scene_actor_availability.h"
#include "scene/motion/scene_actor_chase_reaction.h"
#include "monsters/tentacle_spread.h"
#include "npc/cutscene_actor/actor_animation_controller.h"
#include "engine/engine.h"
#include "link/session/room_actor.h"
/* Complete allocation used by Centipede_Create; private roles are tentative. */
struct TentacleBody {
    struct RoomActor base;
    struct TentacleAnimationSegment segments[4];
    u8 unknown874, spin;
    u8 unknown876[3];
    u8 blockedFrameCount;
    u16 returnCell;
    u8 unknown87C[0x54];
    u16 attackFlags;
    u8 unknown8D2[2];
    s8 oscillationDirection;
    u8 unknown8D5;
    s16 hitReactionTimer;
    s16 oscillationPhase;
    s16 segmentCount;
    u8 unknown8DC[8];
    u16 routeMotionTimer;
    u16 releaseFrame;
    u8 wanderAttempts, wanderDirection, spreadStage;
    u8 unknown8EB;
    s32 pendingImpulse;
    struct TentacleAnimationSegment *impulseSegment;
    u16 unknown8F4;
    u8 unknown8F6[0xE];
    struct ContactPosition waypoint;
    u8 unknown90C[0x18];
    struct ContactPosition returnCenter, savedPosition;
};
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct RoomActor *);
void Centipede_EnterHitReaction(struct ChaseActor *);
void CentipedeBody_GetDirectionFrame(u8, u32 *, u32 *);
void CentipedeBody_ResetDirectionalAnimations(void *, s32);
void CentipedeBody_RefreshDirectionalAnimations(void *, s32);
void Centipede_AssignSegmentPhases(void *);
void ActorTrail_RestoreSavedState(struct ChaseActor *s);
void ActorTrail_ResetAtSavedPosition(struct ChaseActor *);
void SceneActorPalette_ClearCompletedTransitions(void);
void CentipedeBody_RestartSegmentAnimations(void *, u32, u32, u32);
void CentipedeBody_UpdateSpreadRelease(struct ChaseActor *actor, s32 frame);
void CentipedeBody_UpdateChargeSpread(struct ChaseActor *actor, s32 frame);
void ActorTrail_ApplyAngularOffsets(void *, s32, s32, s32);
void CentipedeBody_BeginDirectedAttack(struct ChaseActor *actor);
void CentipedeBody_UpdateAttackRecovery(struct ChaseActor *actor, s32 frame);
void SceneActor_AimArcAtPlayer(void *);
void CentipedeBody_OnAttackCollision(u8 *unused, struct ContactCollider *obj,
                                    struct ChaseActor *ctx);
void CentipedeBody_OnBodyContact(struct SceneActorBehaviorContactView *source,
                                struct SceneActorBehaviorContactView *destination,
                                struct ChaseActor *actor);
void CentipedeBody_UpdateAttackWindup(struct RoomActor *actor);
void CentipedeBody_ResetDirectionalPose(struct RoomActor *actor);
void CentipedeBody_OnIdleState(void);
void CentipedeBody_UpdateInactiveState(void);
void CentipedeBody_UpdateTimedAction(struct ChaseActor *p, s32 time);
void CentipedeBody_UpdateHiddenTransition(struct ChaseActor *s, s32 arg, u32 unused);
void CentipedeBody_UpdateAnimationEleven(struct ChaseActor *p);
void CentipedeBody_UpdateSpreadWindup(struct ChaseActor *actor);
void CentipedeBody_UpdateSpreadPreparation(struct ChaseActor *actor);

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
        Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct AnimSprite *tile =
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
