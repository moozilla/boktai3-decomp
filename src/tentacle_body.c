#include "tentacle_body.h"

void TentacleBody_OnAttackCollision(u8 *unused, struct ContactCollider *collider,
                                    struct ChaseActor *actor)
{
    int mask;
    u32 *flags;
    if ((SceneActor_ActionsBlocked(actor) == 0) && (collider->contactFlags & 0xF)) {
        actor->base.counter304 = collider->collisionId;
        flags = &actor->base.flags2D0;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

static inline u8 TentacleBody_IsUnavailable(struct ChaseActor *actor)
{
    return SceneActorPlacement_IsUnavailable((struct SceneActorPlacementView *)actor);
}

void TentacleBody_OnBodyContact(struct SceneActorBehaviorContactView *source,
                                struct SceneActorBehaviorContactView *destination,
                                struct ChaseActor *actor)
{
    if (TentacleBody_IsUnavailable(actor))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    TentacleActor_EnterHitReaction(actor);
}

void TentacleBody_UpdateAttackWindup(struct RoomActor *actor)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);
    u32 variant, flags;
    if (TentacleSpread_Take(&actor->field2B2)) {
        TentacleBody_GetDirectionFrame(actor->body.direction, &variant, &flags);
        TentacleBody_SelectAnimation(actor, entry[14].value, variant, flags);
        TentacleBody_RefreshDirectionalAnimations(((struct ChaseActor *)actor)->base.context, 15);
        TentacleActor_AssignSegmentPhases(((struct ChaseActor *)actor)->base.context);
    } else {
        TentacleBody_GetDirectionFrame(actor->body.direction, &variant, &flags);
        TentacleBody_SelectAnimation(actor, entry[14].value, variant, flags);
        TentacleBody_RefreshDirectionalAnimations(((struct ChaseActor *)actor)->base.context, 15);
    }
}

void TentacleBody_ResetDirectionalPose(struct RoomActor *actor)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);
    u32 variant, flags;
    if (actor->field2B2)
        actor->field2B2 = 0;
    TentacleBody_GetDirectionFrame(actor->body.direction, &variant, &flags);
    TentacleBody_SelectAnimation(actor, entry[17].value, variant, flags);
    TentacleBody_ResetDirectionalAnimations(((struct ChaseActor *)actor)->base.context, 18);
}

void TentacleBody_OnIdleState(void)
{
}

/* TentacleBody_UpdateInactiveState: Empty state handler adjacent to the tentacle body callbacks; inactive role is tentative */

void TentacleBody_UpdateInactiveState(void)
{
}

/* TentacleBody_UpdateTimedAction: Starts animation 7 and advances action flags at frame 40 and after frame 120 */

static inline void TentacleBody_ClearActionFlags(u16 *f, s32 m)
{
    *f &= m;
}

static inline void TentacleBody_SetActionFlags(u16 m, u16 *f)
{
    *f |= m;
}

void TentacleBody_UpdateTimedAction(struct ChaseActor *actor, s32 frame)
{
    if (TentacleSpread_Take(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 7);
        TentacleBody_ClearActionFlags(&actor->base.flags2D4, ~1);
    }
    if (frame == 40)
        TentacleBody_SetActionFlags(16, &actor->base.flags2D6);
    if (actor->base.restart && frame > 120)
        actor->base.field2B0 = 1;
}

void TentacleBody_UpdateHiddenTransition(struct ChaseActor *actor, s32 frame, u32 unused)
{
    u32 queued = TentacleSpread_Take(&actor->base.field2B2);
    if (queued) {
        u32 mask = 1;
        if (actor->base.drawableKind == 0) {
            struct ActorSprite *drawable = actor->base.drawable;
            u32 value = drawable->flags;
            drawable->flags = value | mask;
        } else {
            struct ActorSprite *drawable = actor->base.drawable;
            struct SceneActorAnimationTile *tile =
                &((struct SceneActorAnimationTileDrawable *)drawable)->tile;
            u32 value = tile->flags;
            tile->flags = value | mask;
        }
        ActorShadow_Disable(actor->base.shadow);
    } else if (frame > 0x1A) {
        void (*nextState)(struct ChaseActor *);
        u32 stateKind;
        u32 *flags;
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry(actor, 9);
        nextState = ActorTrail_RestoreSavedState;
        stateKind = 0xD;
        actor->base.field2B2 = 1;
        actor->base.field2B0 = queued;
        actor->base.field2B5 = stateKind;
        actor->base.target = nextState;
        actor->base.field2C8 = queued;
        flags = &actor->base.flags114;
        mask = ~1;
        *flags &= mask;
        actor->base.word104 = queued;
    } else {
        actor->base.restart = queued;
    }
}

/* Restore the saved control snapshot and play the recovery sound at animation 3, frame 4. */

/* The accessor preserves the saved-state reset code generation with agbcc. */

static inline u8 TentacleBody_IsAnimationReady(struct ChaseActor *actor)
{
    return actor->base.restart;
}

void ActorTrail_RestoreSavedState(struct ChaseActor *actor)
{
    s32 health;
    if (TentacleSpread_Take(&actor->base.field2B2)) {
        ActorShadow_Enable(actor->base.shadow);
        if (!actor->base.drawableKind) {
            u32 *q = &((struct ActorSprite *)actor->base.drawable)->flags;
            u32 mask = ~1;
            *q = mask & *q;
        } else {
            struct SceneActorAnimationTile *q =
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
            &((struct TentacleBodyDrawable *)actor->base.drawable)->sequence;
        if (sequence->index == 3 && sequence->elapsed == 4)
            Sound_PlaySong(0xEE);
    }
    if (!TentacleBody_IsAnimationReady(actor))
        return;
    {
        ActorTrail_ResetAtSavedPosition(actor);
        actor->base.flags2CC = actor->base.saved.behaviorFlags;
        actor->base.flags2D0 = actor->base.saved.statusFlags;
        actor->base.body.direction = actor->base.saved.direction;
        actor->base.state2A8 = actor->base.saved.state;
        actor->base.phase2A9 = actor->base.saved.phase;
        actor->base.behavior2AA = actor->base.saved.behavior;
        actor->base.target = actor->base.saved.control;
        actor->base.field2B5 = actor->base.saved.kind;
        actor->base.flags2C0 = actor->base.saved.motionFlags;
        actor->base.field2EA = actor->base.saved.heightOffset;
        health = actor->base.defaultHealth;
        if (health <= 0)
            health = 0;
        actor->base.health = health;
        actor->base.flags2D6 = 0;
        actor->base.flags2D4 = 0;
        actor->base.field2B0 = 0;
        actor->base.field2B1 = 1;
        actor->base.field2B2 = 1;
        actor->base.restart = 0;
        actor->base.counter304 = 0;
        actor->base.reactionDirection = 0;
        actor->base.field2C2 = 64;
        actor->base.field2D8 = 0;
        actor->base.field2DC = 0;
        actor->base.field2E0 = 0;
        actor->base.field2E6 = 0;
        actor->base.field3CC = 0;
        actor->base.tracking.target = 0;
        SceneActorPalette_ClearCompletedTransitions();
    }
}

/* TentacleBody_UpdateAnimationEleven: Starts directional animation 11 on state entry and completes when the ready flag is set */

void TentacleBody_UpdateAnimationEleven(struct ChaseActor *actor)
{
    if (TentacleSpread_Take(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(actor, 11);
    if (actor->base.restart)
        actor->base.field2B0 = 1;
}

static inline void TentacleBody_SetAttackFlag(struct TentacleBody *body, u32 input)
{
    u32 mask = input;
    u16 *f = &body->attackFlags;
    mask |= *f;
    *f = mask;
}

static inline void TentacleBody_FillMotion(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}

// confidence=low: winds up a segment-spread attack while backing away from the target.

void TentacleBody_UpdateSpreadWindup(struct ChaseActor *actor)
{
    struct TentacleBody *body = actor->base.context;
    s32 activeSegment, segmentIndex, threshold, speed;
    u32 zero;
    struct {
        u32 zero, variant, flags;
    } params;
    if (TentacleSpread_Take(&actor->base.field2B2)) {
        {
            u32 mask = 0x100000;
            actor->base.flags2D0 |= mask;
        }
        TentacleBody_GetDirectionFrame(actor->base.body.direction, &params.variant, &params.flags);
        TentacleBody_RestartSegmentAnimations(body, 18, params.variant, params.flags);
        TentacleBody_SetAttackFlag(body, 2);
    }
    {
        struct ContactPosition *destination = &actor->base.secondaryMotion;
        zero = 0;
        TentacleBody_FillMotion(destination, &params.zero, zero);
    }
    {
        u32 count = body->spreadStage;
        threshold = 200;
        if (count > 3)
            threshold = 300;
    }
    if ((s16)actor->base.field2EA >= threshold) {
        void *target = TentacleBody_UpdateSpreadRelease;
        u32 kind = 39;
        actor->base.field2B2 = 1;
        actor->base.field2B0 = zero;
        actor->base.field2B5 = kind;
        actor->base.target = target;
        actor->base.field2C8 = zero;
        {
            u32 *f = &actor->base.flags114;
            s32 mask = -2;
            *f &= mask;
        }
        actor->base.word104 = zero;
    } else {
        u32 angle;
        if ((s32)actor->base.counter2AC <= 29) {
            speed = 1;
            actor->base.field2EA += 10;
        } else {
            speed = 2;
            actor->base.field2EA += 40;
        }
        angle = (actor->base.body.direction + 128) & 255;
        actor->base.body.motion.x +=
            TentacleSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
        actor->base.body.motion.z += TentacleSpread_Shift(gUnk_086149C4[angle] * speed, 12);
        sub_080A9DB0(actor, 14, 12000);
        activeSegment = 0;
        for (segmentIndex = 0; segmentIndex < body->segmentCount; segmentIndex++) {
            if (TentacleSpread_Test(&body->segments[segmentIndex], 2) &&
                !TentacleSpread_Test(&body->segments[segmentIndex], 5)) {
                switch (activeSegment) {
                    case 0:
                        body->segments[segmentIndex].position.x =
                            (u16)actor->base.body.position.x -
                            TentacleSpread_Shift(
                                gUnk_086149C4[(actor->base.body.direction + 64) & 255] * 54, 12);
                        body->segments[segmentIndex].position.z =
                            (u16)actor->base.body.position.z -
                            TentacleSpread_Shift(gUnk_086149C4[actor->base.body.direction] * 54,
                                                 12);
                        break;
                    case 1:
                        body->segments[segmentIndex].position.x =
                            (u16)actor->base.body.position.x -
                            TentacleSpread_Shift(
                                gUnk_086149C4[(actor->base.body.direction + 64) & 255] * 20, 12);
                        body->segments[segmentIndex].position.z =
                            (u16)actor->base.body.position.z -
                            TentacleSpread_Shift(gUnk_086149C4[actor->base.body.direction] * 20,
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

void TentacleBody_UpdateSpreadRelease(struct ChaseActor *actor, s32 frame)
{
    struct TentacleBody *body = actor->base.context;
    s32 activeSegment, segmentIndex;
    u32 angle, scale;
    u8 *direction;
    if (TentacleSpread_Take(&actor->base.field2B2))
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
        scale = TentacleSpread_Scale(raw);
        direction = field;
    }
    actor->base.body.motion.x +=
        TentacleSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    actor->base.body.motion.z += TentacleSpread_Shift(gUnk_086149C4[angle] * scale, 12);
    body->spin += 12;
    activeSegment = 0;
    for (segmentIndex = 0; segmentIndex < body->segmentCount; segmentIndex++) {
        if (TentacleSpread_Test(&body->segments[segmentIndex], 2) &&
            !TentacleSpread_Test(&body->segments[segmentIndex], 5)) {
            TENTACLE_SPREAD_SET_OFFSET(body->segments[segmentIndex].offset, actor->base.body.motion,
                                       segmentIndex + 1);
            switch (activeSegment) {
                case 0:
                    body->segments[segmentIndex].position.x =
                        (u16)actor->base.body.position.x -
                        TentacleSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 54, 12);
                    body->segments[segmentIndex].position.z =
                        (u16)actor->base.body.position.z -
                        TentacleSpread_Shift(gUnk_086149C4[*direction] * 54, 12);
                    break;
                case 1:
                    body->segments[segmentIndex].position.x =
                        (u16)actor->base.body.position.x -
                        TentacleSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 20, 12);
                    body->segments[segmentIndex].position.z =
                        (u16)actor->base.body.position.z -
                        TentacleSpread_Shift(gUnk_086149C4[*direction] * 20, 12);
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
        actor->base.field2B0 = 1;
        {
            u16 *p = &actor->base.field2E8;
            u32 two = 2;
            *p = two;
            {
                u16 *flags = &body->attackFlags;
                s32 mask = two - 5;
                *flags = mask & *flags;
            }
        }
    } else
        sub_080A9DB0(actor, 64, 0xFFFF);
}

/* Tentative: winds up a segment-spread attack while backing away from the target. */

// confidence=low: winds up a segment-spread attack while backing away from the target.

static inline void TentacleBody_ClearMotion(const u32 *source, void *destination)
{
    CpuSet(source, destination, 0x05000002);
}

void TentacleBody_UpdateSpreadPreparation(struct ChaseActor *actor)
{
    u32 zero;
    struct ContactPosition *dst;
    s32 speed;
    s32 angle;

    if (TentacleSpread_Take(&actor->base.field2B2)) {
        u32 mask = 0x100000;
        actor->base.flags2D0 |= mask;
    }
    dst = &actor->base.secondaryMotion;
    zero = 0;
    TentacleBody_ClearMotion(&zero, dst);
    if ((s16)actor->base.field2EA > 255) {
        void *target = (void *)TentacleBody_UpdateChargeSpread;
        u32 kind = 0x23;
        actor->base.field2B2 = 1;
        actor->base.field2B0 = 0;
        actor->base.field2B5 = kind;
        actor->base.target = target;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.word104 = 0;
        return;
    }
    if ((s32)actor->base.counter2AC <= 29) {
        speed = 1;
        actor->base.field2EA += 10;
    } else {
        speed = 2;
        actor->base.field2EA += 40;
    }
    angle = (actor->base.body.direction + 128) & 255;
    actor->base.body.motion.x +=
        TentacleSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    actor->base.body.motion.z += TentacleSpread_Shift(gUnk_086149C4[angle] * speed, 12);
    sub_080A9DB0(actor, 10, 0x1000);
    ActorTrail_ApplyAngularOffsets(actor, 20, 24, 0);
}

// confidence=low: spreads the body's segments out from the actor's position, then attacks.

void TentacleBody_UpdateChargeSpread(struct ChaseActor *actor, s32 frame)
{
    struct TentacleBody *body = actor->base.context;
    s32 activeSegment, segmentIndex;
    u32 angle, scale;
    u8 *direction;
    if (TentacleSpread_Take(&actor->base.field2B2))
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
        scale = TentacleSpread_Scale(raw);
        direction = field;
    }
    actor->base.body.motion.x +=
        TentacleSpread_Shift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    actor->base.body.motion.z += TentacleSpread_Shift(gUnk_086149C4[angle] * scale, 12);
    body->spin += 12;
    activeSegment = 0;
    for (segmentIndex = 0; segmentIndex < body->segmentCount; segmentIndex++) {
        if (TentacleSpread_Test(&body->segments[segmentIndex], 2) &&
            !TentacleSpread_Test(&body->segments[segmentIndex], 5)) {
            TENTACLE_SPREAD_SET_OFFSET(body->segments[segmentIndex].offset, actor->base.body.motion,
                                       segmentIndex + 1);
            switch (activeSegment) {
                case 0:
                    body->segments[segmentIndex].position.x =
                        (u16)actor->base.body.position.x -
                        TentacleSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 54, 12);
                    body->segments[segmentIndex].position.z =
                        (u16)actor->base.body.position.z -
                        TentacleSpread_Shift(gUnk_086149C4[*direction] * 54, 12);
                    break;
                case 1:
                    body->segments[segmentIndex].position.x =
                        (u16)actor->base.body.position.x -
                        TentacleSpread_Shift(gUnk_086149C4[(*direction + 64) & 255] * 20, 12);
                    body->segments[segmentIndex].position.z =
                        (u16)actor->base.body.position.z -
                        TentacleSpread_Shift(gUnk_086149C4[*direction] * 20, 12);
                    break;
                default:
                    body->segments[segmentIndex].position.x = (u16)actor->base.body.position.x;
                    body->segments[segmentIndex].position.z = (u16)actor->base.body.position.z;
                    break;
            }
            activeSegment++;
        }
    }
    sub_080A9DB0(actor, 64, 0xFFFF);
    if (frame > 60)
        SceneActorChase_SetControl(actor, (void *)TentacleBody_BeginDirectedAttack, 36);
}

void TentacleBody_BeginDirectedAttack(struct ChaseActor *actor)
{
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        ((struct RoomActor *)actor)->profileId, ((struct RoomActor *)actor)->profileMode, 0);
    struct TentacleBody *body = actor->base.context;
    u32 variant;
    u32 flags;
    u32 entered = TentacleSpread_Take(&actor->base.field2B2);
    u8 *direction = &actor->base.body.direction;
    u32 *attackFlags = &actor->base.flags2D0;
    if (entered) {
        SceneActor_AimArcAtPlayer(actor);
        *direction = Math_Atan2(actor->base.secondaryMotion.x, actor->base.secondaryMotion.z);
        {
            u32 mask = 256;
            actor->base.flags2D4 |= mask;
        }
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
    TentacleBody_GetDirectionFrame(*direction, &variant, &flags);
    TentacleBody_SelectAnimation((void *)actor, entry[14].value, variant, flags);
    ActorTrail_ApplyAngularOffsets(actor, 18, 6, 0);
    TentacleBody_RefreshDirectionalAnimations(actor->base.context, 15);
    {
        u32 mask = 16;
        if ((*attackFlags) & mask) {
            {
                u16 *field = &actor->base.flags2D4;
                s32 mask = ~256;
                actor->base.flags2D4 = *field & mask;
            }
            {
                void *callback = (void *)TentacleBody_UpdateAttackRecovery;
                u32 kind = 37;
                actor->base.field2B2 = 1;
                actor->base.field2B0 = 0;
                actor->base.field2B5 = kind;
                actor->base.target = callback;
                actor->base.field2C8 = 0;
                {
                    u32 *flags = &actor->base.flags114;
                    u32 mask = ~1;
                    *flags &= mask;
                }
                actor->base.word104 = 0;
            }
        } else {
            u32 mask = 8192;
            actor->base.flags2CC |= mask;
        }
    }
}

void TentacleBody_UpdateAttackRecovery(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        ((struct RoomActor *)actor)->profileId, ((struct RoomActor *)actor)->profileMode, 0);
    u32 variant, flags;
    if (actor->base.field2B2)
        actor->base.field2B2 = 0;
    TentacleBody_GetDirectionFrame(actor->base.body.direction, &variant, &flags);
    TentacleBody_SelectAnimation((void *)actor, entry[17].value, variant, flags);
    TentacleBody_ResetDirectionalAnimations(actor->base.context, 18);
    sub_080A9DB0(actor, 20, 25600);
    if (frame > 40) {
        TentacleBody_GetDirectionFrame(actor->base.body.direction, &variant, &flags);
        TentacleBody_SelectAnimation((void *)actor, entry[14].value, variant, flags);
        TentacleBody_RefreshDirectionalAnimations(actor->base.context, 15);
        actor->base.field2B0 = 1;
    }
}
