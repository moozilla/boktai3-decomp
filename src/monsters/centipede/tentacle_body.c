#include "monsters/centipede/centipede_body.h"

#include "scene/actor/scene_actor_init_actor.h"
#include "engine/word_flags.h"

void CentipedeBody_OnAttackCollision(u8 *unused, struct ContactCollider *collider,
                                    struct ChaseActor *actor)
{
    int mask;
    u32 *flags;
    if ((SceneActor_ActionsBlocked(actor) == 0) && (collider->contactFlags & 0xF)) {
        actor->base.counter304 = collider->collisionId;
        flags = &actor->base.statusFlags;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

static inline u8 CentipedeBody_IsUnavailable(struct ChaseActor *actor)
{
    return SceneActorPlacement_IsUnavailable(&actor->base);
}

void CentipedeBody_OnBodyContact(struct SceneActorBehaviorContactView *source,
                                struct SceneActorBehaviorContactView *destination,
                                struct ChaseActor *actor)
{
    if (CentipedeBody_IsUnavailable(actor))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    Centipede_EnterHitReaction(actor);
}

void CentipedeBody_UpdateAttackWindup(struct RoomActor *actor)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);
    u32 variant, flags;
    if (CentipedeSpread_Take(&actor->field2B2)) {
        CentipedeBody_GetDirectionFrame(actor->body.direction, &variant, &flags);
        CentipedeBody_SelectAnimation(actor, entry[14].value, variant, flags);
        CentipedeBody_RefreshDirectionalAnimations(((struct ChaseActor *)actor)->base.context, 15);
        Centipede_AssignSegmentPhases(((struct ChaseActor *)actor)->base.context);
    } else {
        CentipedeBody_GetDirectionFrame(actor->body.direction, &variant, &flags);
        CentipedeBody_SelectAnimation(actor, entry[14].value, variant, flags);
        CentipedeBody_RefreshDirectionalAnimations(((struct ChaseActor *)actor)->base.context, 15);
    }
}

void CentipedeBody_ResetDirectionalPose(struct RoomActor *actor)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);
    u32 variant, flags;
    if (actor->field2B2)
        actor->field2B2 = 0;
    CentipedeBody_GetDirectionFrame(actor->body.direction, &variant, &flags);
    CentipedeBody_SelectAnimation(actor, entry[17].value, variant, flags);
    CentipedeBody_ResetDirectionalAnimations(((struct ChaseActor *)actor)->base.context, 18);
}

void CentipedeBody_OnIdleState(void)
{
}

/* CentipedeBody_UpdateInactiveState: Empty state handler adjacent to the tentacle body callbacks; inactive role is tentative */

void CentipedeBody_UpdateInactiveState(void)
{
}

/* CentipedeBody_UpdateTimedAction: Starts animation 7 and advances action flags at frame 40 and after frame 120 */

void CentipedeBody_UpdateTimedAction(struct ChaseActor *actor, s32 frame)
{
    if (CentipedeSpread_Take(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 7);
        HalfwordFlags_And(&actor->base.motionFlags, ~1);
    }
    if (frame == 40)
        HalfwordFlags_Set(16, &actor->base.reactionFlags);
    if (actor->base.restart && frame > 120)
        actor->base.stepDone = 1;
}

void CentipedeBody_UpdateHiddenTransition(struct ChaseActor *actor, s32 frame, u32 unused)
{
    u32 queued = CentipedeSpread_Take(&actor->base.field2B2);
    if (queued) {
        u32 mask = 1;
        if (actor->base.drawableKind == 0) {
            struct ActorSprite *drawable = actor->base.drawable;
            u32 value = drawable->flags;
            drawable->flags = value | mask;
        } else {
            struct ActorSprite *drawable = actor->base.drawable;
            struct AnimSprite *tile =
                &((struct SceneActorAnimationTileDrawable *)drawable)->tile;
            u32 value = tile->flags;
            tile->flags = value | mask;
        }
        ActorShadow_Disable(actor->base.shadow);
    } else if (frame > 0x1A) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 9);
        SceneActorChase_SetControl(actor, ActorTrail_RestoreSavedState, 13);
    } else {
        actor->base.restart = queued;
    }
}

/* Restore the saved control snapshot and play the recovery sound at animation 3, frame 4. */

/* The accessor preserves the saved-state reset code generation with agbcc. */

static inline u8 CentipedeBody_IsAnimationReady(struct ChaseActor *actor)
{
    return actor->base.restart;
}

void ActorTrail_RestoreSavedState(struct ChaseActor *actor)
{
    s32 health;
    if (CentipedeSpread_Take(&actor->base.field2B2)) {
        ActorShadow_Enable(actor->base.shadow);
        if (!actor->base.drawableKind) {
            u32 *q = &((struct ActorSprite *)actor->base.drawable)->flags;
            u32 mask = ~1;
            *q = mask & *q;
        } else {
            struct AnimSprite *q =
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile;
            u32 mask = ~1;
            q->flags = mask & q->flags;
        }
        {
            u16 one = 1;
            actor->base.health = one;
        }
        actor->base.colliderA.responseValue = 60;
    }
    {
        struct DisplaySequenceState *sequence =
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
        if (sequence->index == 3 && sequence->elapsed == 4)
            Sound_PlaySong(0xEE);
    }
    if (!CentipedeBody_IsAnimationReady(actor))
        return;
    {
        ActorTrail_ResetAtSavedPosition(actor);
        actor->base.behaviorFlags = actor->base.saved.behaviorFlags;
        actor->base.statusFlags = actor->base.saved.statusFlags;
        actor->base.body.direction = actor->base.saved.direction;
        actor->base.state2A8 = actor->base.saved.state;
        actor->base.phase2A9 = actor->base.saved.phase;
        actor->base.behavior2AA = actor->base.saved.behavior;
        actor->base.target = actor->base.saved.control;
        actor->base.controlKind = actor->base.saved.kind;
        actor->base.placementFlags = actor->base.saved.motionFlags;
        actor->base.heightOffset = actor->base.saved.heightOffset;
        health = actor->base.defaultHealth;
        if (health <= 0)
            health = 0;
        actor->base.health = health;
        actor->base.reactionFlags = 0;
        actor->base.motionFlags = 0;
        actor->base.stepDone = 0;
        actor->base.field2B1 = 1;
        actor->base.field2B2 = 1;
        actor->base.restart = 0;
        actor->base.counter304 = 0;
        actor->base.reactionDirection = 0;
        actor->base.field2C2 = 64;
        actor->base.hitAttributes = 0;
        actor->base.hitReactionFlags = 0;
        actor->base.hitOwners = 0;
        actor->base.field2E6 = 0;
        actor->base.field3CC = 0;
        actor->base.tracking.target = 0;
        SceneActorPalette_ClearCompletedTransitions();
    }
}

/* CentipedeBody_UpdateAnimationEleven: Starts directional animation 11 on state entry and completes when the ready flag is set */

void CentipedeBody_UpdateAnimationEleven(struct ChaseActor *actor)
{
    if (CentipedeSpread_Take(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(actor, 11);
    if (actor->base.restart)
        actor->base.stepDone = 1;
}

static inline void CentipedeBody_SetAttackFlag(struct TentacleBody *body, u32 input)
{
    u32 mask = input;
    u16 *f = &body->attackFlags;
    mask |= *f;
    *f = mask;
}

// confidence=low: winds up a segment-spread attack while backing away from the target.

void CentipedeBody_UpdateSpreadWindup(struct ChaseActor *actor)
{
    struct TentacleBody *body = actor->base.context;
    s32 activeSegment, segmentIndex, threshold, speed;
    u32 variant, flags;
    if (CentipedeSpread_Take(&actor->base.field2B2)) {
        MonsterInit_SetStatusFlags(actor, 0x100000);
        CentipedeBody_GetDirectionFrame(actor->base.body.direction, &variant, &flags);
        CentipedeBody_RestartSegmentAnimations(body, 18, variant, flags);
        CentipedeBody_SetAttackFlag(body, 2);
    }
    ZeroFill8(&actor->base.secondaryMotion);
    {
        u32 count = body->spreadStage;
        threshold = 200;
        if (count > 3)
            threshold = 300;
    }
    if ((s16)actor->base.heightOffset >= threshold) {
        SceneActorChase_SetControl(actor, CentipedeBody_UpdateSpreadRelease, 39);
    } else {
        u32 angle;
        if ((s32)actor->base.counter2AC <= 29) {
            speed = 1;
            actor->base.heightOffset += 10;
        } else {
            speed = 2;
            actor->base.heightOffset += 40;
        }
        angle = (actor->base.body.direction + 128) & 255;
        actor->base.body.motion.x +=
            CentipedeSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
        actor->base.body.motion.z += CentipedeSpread_Shift(gUnk_086149C4[angle] * speed, 12);
        Centipede_UpdateSegmentPositions(actor, 14, 12000);
        activeSegment = 0;
        for (segmentIndex = 0; segmentIndex < body->segmentCount; segmentIndex++) {
            if (CentipedeSpread_Test(&body->segments[segmentIndex], 2) &&
                !CentipedeSpread_Test(&body->segments[segmentIndex], 5)) {
                switch (activeSegment) {
                    case 0:
                        body->segments[segmentIndex].position.x =
                            (u16)actor->base.body.position.x -
                            CentipedeSpread_Shift(
                                gUnk_086149C4[(actor->base.body.direction + 64) & 255] * 54, 12);
                        body->segments[segmentIndex].position.z =
                            (u16)actor->base.body.position.z -
                            CentipedeSpread_Shift(gUnk_086149C4[actor->base.body.direction] * 54,
                                                 12);
                        break;
                    case 1:
                        body->segments[segmentIndex].position.x =
                            (u16)actor->base.body.position.x -
                            CentipedeSpread_Shift(
                                gUnk_086149C4[(actor->base.body.direction + 64) & 255] * 20, 12);
                        body->segments[segmentIndex].position.z =
                            (u16)actor->base.body.position.z -
                            CentipedeSpread_Shift(gUnk_086149C4[actor->base.body.direction] * 20,
                                                 12);
                        break;
                    default:
                        body->segments[segmentIndex].position.x = (u16)actor->base.body.position.x;
                        body->segments[segmentIndex].position.z = (u16)actor->base.body.position.z;
                        break;
                }
                activeSegment++;
            }
        }
    }
}

// confidence=low: spreads the body's segments out from the actor's position, then releases.

void CentipedeBody_UpdateSpreadRelease(struct ChaseActor *actor, s32 frame)
{
    struct TentacleBody *body = actor->base.context;
    s32 activeSegment, segmentIndex;
    u32 angle, scale;
    u8 *direction;
    if (CentipedeSpread_Take(&actor->base.field2B2))
        body->spin = 0;
    {
        u16 raw = body->spin;
        u8 *field;
        if ((s8)body->spin >= 0) {
            field = &actor->base.body.direction;
            angle = *field;
        } else {
            field = &actor->base.body.direction;
            angle = (*field + 128) & 255;
        }
        scale = CentipedeSpread_Scale(raw);
        direction = field;
    }
    actor->base.body.motion.x +=
        CentipedeSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    actor->base.body.motion.z += CentipedeSpread_Shift(gUnk_086149C4[angle] * scale, 12);
    body->spin += 12;
    activeSegment = 0;
    for (segmentIndex = 0; segmentIndex < body->segmentCount; segmentIndex++) {
        if (CentipedeSpread_Test(&body->segments[segmentIndex], 2) &&
            !CentipedeSpread_Test(&body->segments[segmentIndex], 5)) {
            CENTIPEDE_SPREAD_SET_OFFSET(body->segments[segmentIndex].offset, actor->base.body.motion,
                                       segmentIndex + 1);
            switch (activeSegment) {
                case 0:
                    body->segments[segmentIndex].position.x =
                        (u16)actor->base.body.position.x -
                        CentipedeSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 54, 12);
                    body->segments[segmentIndex].position.z =
                        (u16)actor->base.body.position.z -
                        CentipedeSpread_Shift(gUnk_086149C4[*direction] * 54, 12);
                    break;
                case 1:
                    body->segments[segmentIndex].position.x =
                        (u16)actor->base.body.position.x -
                        CentipedeSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 20, 12);
                    body->segments[segmentIndex].position.z =
                        (u16)actor->base.body.position.z -
                        CentipedeSpread_Shift(gUnk_086149C4[*direction] * 20, 12);
                    break;
                default:
                    body->segments[segmentIndex].position.x = (u16)actor->base.body.position.x;
                    body->segments[segmentIndex].position.z = (u16)actor->base.body.position.z;
                    break;
            }
            activeSegment++;
        }
    }
    if (frame > body->releaseFrame) {
        actor->base.stepDone = 1;
        actor->base.field2E8 = 2;
        SceneActorChase_ClearFlagComponent((void *)&body->attackFlags, 2);
    } else
        Centipede_UpdateSegmentPositions(actor, 64, 0xFFFF);
}

/* Tentative: winds up a segment-spread attack while backing away from the target. */

// confidence=low: winds up a segment-spread attack while backing away from the target.

void CentipedeBody_UpdateSpreadPreparation(struct ChaseActor *actor)
{
    s32 speed;
    s32 angle;

    if (CentipedeSpread_Take(&actor->base.field2B2)) {
        MonsterInit_SetStatusFlags(actor, 0x100000);
    }
    ZeroFill8(&actor->base.secondaryMotion);
    if ((s16)actor->base.heightOffset > 255) {
        SceneActorChase_SetControl(actor, CentipedeBody_UpdateChargeSpread, 35);
        return;
    }
    if ((s32)actor->base.counter2AC <= 29) {
        speed = 1;
        actor->base.heightOffset += 10;
    } else {
        speed = 2;
        actor->base.heightOffset += 40;
    }
    angle = (actor->base.body.direction + 128) & 255;
    actor->base.body.motion.x +=
        CentipedeSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    actor->base.body.motion.z += CentipedeSpread_Shift(gUnk_086149C4[angle] * speed, 12);
    Centipede_UpdateSegmentPositions(actor, 10, 0x1000);
    ActorTrail_ApplyAngularOffsets(actor, 20, 24, 0);
}

// confidence=low: spreads the body's segments out from the actor's position, then attacks.

void CentipedeBody_UpdateChargeSpread(struct ChaseActor *actor, s32 frame)
{
    struct TentacleBody *body = actor->base.context;
    s32 activeSegment, segmentIndex;
    u32 angle, scale;
    u8 *direction;
    if (CentipedeSpread_Take(&actor->base.field2B2))
        body->spin = 0;
    {
        u8 raw = body->spin;
        u8 *field;
        if ((s8)body->spin >= 0) {
            field = &actor->base.body.direction;
            angle = *field;
        } else {
            field = &actor->base.body.direction;
            angle = (*field + 128) & 255;
        }
        scale = CentipedeSpread_Scale(raw);
        direction = field;
    }
    actor->base.body.motion.x +=
        CentipedeSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    actor->base.body.motion.z += CentipedeSpread_Shift(gUnk_086149C4[angle] * scale, 12);
    body->spin += 12;
    activeSegment = 0;
    for (segmentIndex = 0; segmentIndex < body->segmentCount; segmentIndex++) {
        if (CentipedeSpread_Test(&body->segments[segmentIndex], 2) &&
            !CentipedeSpread_Test(&body->segments[segmentIndex], 5)) {
            CENTIPEDE_SPREAD_SET_OFFSET(body->segments[segmentIndex].offset, actor->base.body.motion,
                                       segmentIndex + 1);
            switch (activeSegment) {
                case 0:
                    body->segments[segmentIndex].position.x =
                        (u16)actor->base.body.position.x -
                        CentipedeSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 54, 12);
                    body->segments[segmentIndex].position.z =
                        (u16)actor->base.body.position.z -
                        CentipedeSpread_Shift(gUnk_086149C4[*direction] * 54, 12);
                    break;
                case 1:
                    body->segments[segmentIndex].position.x =
                        (u16)actor->base.body.position.x -
                        CentipedeSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 20, 12);
                    body->segments[segmentIndex].position.z =
                        (u16)actor->base.body.position.z -
                        CentipedeSpread_Shift(gUnk_086149C4[*direction] * 20, 12);
                    break;
                default:
                    body->segments[segmentIndex].position.x = (u16)actor->base.body.position.x;
                    body->segments[segmentIndex].position.z = (u16)actor->base.body.position.z;
                    break;
            }
            activeSegment++;
        }
    }
    Centipede_UpdateSegmentPositions(actor, 64, 0xFFFF);
    if (frame > 60)
        SceneActorChase_SetControl(actor, (void *)CentipedeBody_BeginDirectedAttack, 36);
}

void CentipedeBody_BeginDirectedAttack(struct ChaseActor *actor)
{
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        actor->base.profileId, actor->base.profileMode, 0);
    struct TentacleBody *body = actor->base.context;
    u32 variant;
    u32 flags;
    u32 entered = CentipedeSpread_Take(&actor->base.field2B2);
    u8 *direction = &actor->base.body.direction;
    u32 *attackFlags = &actor->base.statusFlags;
    if (entered) {
        SceneActor_AimArcAtPlayer(actor);
        *direction = Math_Atan2(actor->base.secondaryMotion.x, actor->base.secondaryMotion.z);
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
        {
            u32 mask = ~0x100000;
            *attackFlags &= mask;
        }
        {
            u16 *bodyFlags = &body->attackFlags;
            s32 mask = ~2;
            u32 value = *bodyFlags;
            mask &= value;
            mask |= 4;
            body->attackFlags = mask;
        }
        Sound_PlaySong(982);
    }
    CentipedeBody_GetDirectionFrame(*direction, &variant, &flags);
    CentipedeBody_SelectAnimation((void *)actor, entry[14].value, variant, flags);
    ActorTrail_ApplyAngularOffsets(actor, 18, 6, 0);
    CentipedeBody_RefreshDirectionalAnimations(actor->base.context, 15);
    {
        u32 mask = 16;
        if ((*attackFlags) & mask) {
            {
                u16 *field = &actor->base.motionFlags;
                s32 mask = ~256;
                actor->base.motionFlags = *field & mask;
            }
            SceneActorChase_SetControl(actor, CentipedeBody_UpdateAttackRecovery, 37);
        } else {
            SceneActorChase_SetActorFlags(actor, 0x2000);
        }
    }
}

void CentipedeBody_UpdateAttackRecovery(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        actor->base.profileId, actor->base.profileMode, 0);
    u32 variant, flags;
    if (actor->base.field2B2)
        actor->base.field2B2 = 0;
    CentipedeBody_GetDirectionFrame(actor->base.body.direction, &variant, &flags);
    CentipedeBody_SelectAnimation((void *)actor, entry[17].value, variant, flags);
    CentipedeBody_ResetDirectionalAnimations(actor->base.context, 18);
    Centipede_UpdateSegmentPositions(actor, 20, 25600);
    if (frame > 40) {
        CentipedeBody_GetDirectionFrame(actor->base.body.direction, &variant, &flags);
        CentipedeBody_SelectAnimation((void *)actor, entry[14].value, variant, flags);
        CentipedeBody_RefreshDirectionalAnimations(actor->base.context, 15);
        actor->base.stepDone = 1;
    }
}
