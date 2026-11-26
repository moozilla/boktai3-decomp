#include "scene/collision/scene_actor_delayed_response.h"
#include "scene/actor/scene_actor_status.h"
#include "scene/actor/scene_actor_placement.h"

typedef char
    Check_attachmentLimit[(u32) & ((struct ChaseActor *)0)->base.attachmentLimit == 0xA6 ? 1 : -1];
typedef char
    Check_attachmentTimer[(u32) & ((struct ChaseActor *)0)->base.attachmentTimer == 0xAA ? 1 : -1];
typedef char Check_alternatePalette[(u32) & ((struct ChaseActor *)0)->base.alternatePalette == 0x1DC
                                        ? 1
                                        : -1];
typedef char Check_flashTimer[(u32) & ((struct ChaseActor *)0)->base.flashTimer == 0x1E8 ? 1 : -1];
typedef char Check_deathControl[(u32) & ((struct ChaseActor *)0)->base.onDefeat == 0x234 ? 1 : -1];
typedef char
    Check_secondaryMotion[(u32) & ((struct ChaseActor *)0)->base.secondaryMotion == 0x2EC ? 1 : -1];
typedef char
    Check_particleTimer[(u32) & ((struct ChaseActor *)0)->base.particleTimer == 0x30C ? 1 : -1];
typedef char Check_route[(u32) & ((struct ChaseActor *)0)->group.homeRoute == 0x448 ? 1 : -1];
typedef char
    Check_contactNode[(u32) & ((struct ChaseActor *)0)->group.contactNode == 0x45C ? 1 : -1];
typedef char Check_contactCooldown[(u32) & ((struct ChaseActor *)0)->group.contactCooldown == 0x5C8
                                       ? 1
                                       : -1];
typedef char Check_effect5CC[(u32) & ((struct ChaseActor *)0)->group.effect == 0x5CC ? 1 : -1];
typedef char
    Check_behaviorControl[(u32) & ((struct ChaseActor *)0)->group.contactControl == 0x5DC ? 1 : -1];
typedef char Check_interactionCallback
    [(u32) & ((struct ChaseActor *)0)->group.interactionCallback == 0x618 ? 1 : -1];
typedef char
    Check_responseCallback[(u32) & ((struct ChaseActor *)0)->group.responseCallback == 0x61C ? 1
                                                                                             : -1];
void SceneActorRecord_RecordPair(u32, u32);
void SceneActorAttachment_ReplaceResponseSlot(void *, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);
s32 Div(s32, s32);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
u32 TetherActor_CanStartInteraction(void);
void SceneActorReaction_RestoreAfterDelay(void);

static inline u32 ActorStatus_HasBehaviorFlags(void *object, u32 mask)
{
    return ActorStatus_BitsPresent(ActorStatus_BehaviorFlags(object, mask));
}

static inline void ActorStatus_SetPlacementFlags(struct SceneActorChasePlacement *placement,
                                                 u32 bits)
{
    placement->flags |= bits;
}
static inline void ActorStatus_ClearPlacementFlags(struct SceneActorChasePlacement *placement,
                                                   u32 bits)
{
    u32 mask = ~bits;
    placement->flags &= mask;
}

static inline void ActorStatus_ResetInteractionAt(struct ChaseActor *actor,
                                                  struct SceneActorRegistryControl *control)
{
    if (ActorStatus_HasBehaviorFlags(actor, 0x200000)) {
        ActorStatus_ClearInteraction(actor, control);
    }
}

static inline void ActorStatus_ResetInteraction(void *object)
{
    struct ChaseActor *actor = object;
    ActorStatus_ResetInteractionAt(actor, (void *)&actor->group.common.control);
}
static inline void ActorStatus_ResetRouteAt(struct ChaseActor *actor,
                                            struct SceneActorRegistryControl *control)
{
    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x141C0);
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}
static inline void ActorStatus_ResetRoute(void *object)
{
    struct ChaseActor *actor = object;
    ActorStatus_ResetRouteAt(actor, (void *)&actor->group.common.control);
}
static inline void ActorStatus_ResetSelection(struct ChaseActor *actor)
{
    actor->group.turn.turnsRemaining = 255;
}

/* shared reset of the knockback/stun scratch state */
static inline void ActorStatus_ResetReaction(struct ChaseActor *actor)
{
    actor->base.field2C2 = 0x40;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    ActorStatus_ClearMotionFlags(actor, 0x380);
    ActorStatus_ClearTrackingFlags((void *)&actor->base.tracking.target, 0x18);
    ActorStatus_ResetInteraction(actor);
    ActorStatus_ClearMotionFlags(actor, 0x400);
    ActorStatus_ResetSelection(actor);
}

/* the dead/downed behaviour block: set action table slots */

/* terrain cell index from a packed position pair */
static inline void ActorStatus_TickRouteControl(struct SceneActorRegistryControl *control)
{
    if (control->timer)
        control->timer--;
}
static inline void ActorStatus_TickAttachment(struct ChaseActor *actor)
{
    s32 remaining = (*(s16 *)&actor->base.attachmentTimer);
    if (remaining <= 0) {
        (*(s16 *)&actor->base.attachmentTimer) = 0;
        if (ActorStatus_HasStatusFlags(actor, 0xC0))
            ActorStatus_SetWord(&actor->base.statusFlags, 0x100);
    } else {
        (*(s16 *)&actor->base.attachmentTimer) = remaining - 1;
    }
    if (actor->group.contactCooldown)
        actor->group.contactCooldown--;
}

static inline u32 ActorStatus_IsUnstable(struct ChaseActor *actor)
{
    u32 result = 0;
    if (ActorStatus_BodyFlags(actor, 0x1000))
        return 1;
    if (ActorStatus_MotionFlags(actor, 0xA03F) || actor->base.colliderA.responseValue != 0 ||
        (*(s16 *)&actor->base.heightOffset) > 0 || (*(u32 *)&actor->base.secondaryMotion.x) != 0 ||
        actor->base.secondaryMotion.z != 0)
        return 1;
    if (ActorStatus_IsDown(actor))
        result = 1;
    return result;
}
static inline void ActorStatus_CollisionGate(void *object)
{
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    if (ActorStatus_BodyFlags(actor, 0x200)) {
        if ((ActorStatus_IsUnstable(actor) && !ActorStatus_MotionFlags(actor, 0x8000)) ||
            ActorStatus_IsDown(actor))
            ActorStatus_SetPlacementFlags(&group->common.placement, 1);
        else
            ActorStatus_ClearPlacementFlags(&group->common.placement, 1);
    }
}

static inline s32 SceneEnvironment_SolarLevel(void)
{
    struct SceneEnvironmentSnapshot result = SceneEnvironment_Read();
    return result.solarLevel;
}

static inline void ActorStatus_EffectSelector(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 v;
    /* region marker at +0x5CC based on level */
    if (ActorStatus_HasBehaviorFlags(actor, 0x40000)) {
        v = SceneEnvironment_SolarLevel();
        if (v <= 2) {
            if (ActorStatus_HasBehaviorFlags(actor, 0x80000))
                group->effect = 0x21;
            else
                group->effect = 0x33;
        } else if (v <= 6) {
            group->effect = 0x32;
        } else {
            if (ActorStatus_HasBehaviorFlags(actor, 0x80000))
                group->effect = 0x33;
            else
                group->effect = 0x21;
        }
    }
}

static inline s16 ActorStatus_AttachmentRestart(struct ChaseActor *actor)
{
    return actor->base.attachmentRestart;
}

static inline void ActorStatus_Wake(struct ChaseActor *actor)
{
    const struct ChaseActor *identity = (void *)actor;

    /* wake-up transition when the stun flag is set */
    if (!(ActorStatus_MotionFlags(actor, 0x40)) && !(ActorStatus_StatusFlags(actor, 0x20000)) &&
        (ActorStatus_BehaviorFlags(actor, 0x1000))) {
        ActorStatus_ResetRoute(actor);
        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x1000);
        actor->base.reactionStart.x = actor->base.body.position.x;
        actor->base.reactionStart.z = actor->base.body.position.z;
        actor->base.reactionEnd.x = actor->base.body.position.x;
        actor->base.reactionEnd.z = actor->base.body.position.z;
        if ((*(s16 *)&actor->base.attachmentTimer) >= actor->base.attachmentLimit)
            (*(s16 *)&actor->base.attachmentTimer) = ActorStatus_AttachmentRestart(actor);
        if (actor->base.health <= 0) {
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, actor->base.onDefeat, 0x11);
            if (!SceneEnvironment_LinkMode() && !ActorStatus_StatusFlags((void *)identity, 0x800))
                SceneActorRecord_RecordPair(
                    gScriptEntryProcedure,
                    (u16)((const struct ChaseActor *)identity)->base.body.id);
        } else if (ActorStatus_BehaviorFlags(actor, 0x200)) {
            SceneActorChase_SetControl(actor, actor->base.onReaction, 0x12);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else {
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0x7F, actor->base.category);
            Callback_InvokeActor(actor, actor->group.responseCallback);
            SceneActorChase_SetControl(actor, actor->group.contactControl, 7);
            SceneActorChase_SetState(actor, 3, 0, 0);
        }
    }
}

static inline void ActorStatus_SetRecoveryDuration(struct ChaseActor *actor, u32 category)
{
    if (category == 0x15 || category == 0x19) {
        actor->base.field2C2 = 0x20;
        return;
    }
    if (category - 5 <= 2) {
        actor->base.field2C2 = 0x20;
        return;
    }
    actor->base.field2C2 = Div(0x40, 3);
}

static inline void ActorStatus_ReactionFlags(void *object)
{
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    u32 cat;
    /* state-flag dispatch */
    if (((struct ChaseActor *)object)->base.motionFlags == 0) {
        SceneActorChase_ClearWordFlags(&((struct ChaseActor *)object)->base.behaviorFlags, 0x2000);
    } else if (ActorStatus_IsDown(object)) {
        ActorStatus_ReleaseContact(object);
        ActorStatus_ResetInteractionAt(actor, &group->common.control);
        actor->base.motionFlags = 0x40;
        actor->base.field2E6 = 0;
        SceneActorChase_ClearBits(&actor->base.reactionFlags, 0x980);
    } else {

        cat = actor->base.category;

        if (ActorStatus_MotionFlags(actor, 0x2000)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->base.tracking.target, 8);
            ActorStatus_ClearMotionFlags(actor, 0x500);
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->base.field2C2 = 0x40;
            SceneActorChase_ClearBits(&actor->base.reactionFlags, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 4)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->base.tracking.target, 8);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->base.field2C2 = 0x40;
            SceneActorChase_ClearBits(&actor->base.reactionFlags, 0x980);
            ActorStatus_SetWord(&actor->base.behaviorFlags, 0x2000);
        } else if (ActorStatus_MotionFlags(actor, 2)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->base.tracking.target, 8);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->base.reactionFlags, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 0x8000)) {
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->base.reactionFlags, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 0x10)) {
            ActorStatus_ResetRouteAt(actor, &group->common.control);
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            (*(s16 *)&actor->base.attachmentTimer) = 0;
            actor->base.field2C2 = Div(0x40, 6);
            ActorStatus_ReleaseContact(object);
            ActorStatus_ResetInteractionAt(actor, &group->common.control);
            ActorStatus_ClearMotionFlags(actor, 0x400);
            group->turn.turnsRemaining |= -1;
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->base.reactionFlags, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 8)) {
            ActorStatus_ResetRouteAt(actor, &group->common.control);
            ActorStatus_ResetInteractionAt(actor, &group->common.control);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->base.reactionFlags, 0x980);
        }

        if (ActorStatus_MotionFlags(actor, 0x803E)) {
            ActorStatus_ClearMotionFlags(actor, 0x201);
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x4000);
            ActorStatus_ClearTrackingFlags((void *)&actor->base.tracking.target, 0x10);
            if (!(ActorStatus_MotionFlags(actor, 0x20)))
                SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x80);
            SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x100004);
            ActorStatus_ReleaseContact(object);
            ActorStatus_ResetInteractionAt(actor, &group->common.control);
            ActorStatus_ClearMotionFlags(actor, 0x400);
            group->turn.turnsRemaining = 0xFF;
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->base.field2E6 = 0;
            if (!(SceneActorChase_ReadMotionFlags(actor) & 0x10))
                ActorStatus_SetWord(&actor->base.behaviorFlags, 0x2000);
            else
                SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x2000);
        } else {
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x2000);
        }

        if (ActorStatus_MotionFlags(actor, 0x400)) {
            if (group->turn.turnsRemaining < 0 ||
                (actor->base.controlKind != 0x1B && actor->base.controlKind != 2)) {
                group->turn.turnsRemaining = 0xFF;
                ActorStatus_ClearMotionFlags(actor, 0x400);
            }
        }
        if (ActorStatus_MotionFlags(actor, 0x80)) {
            ActorStatus_SetRecoveryDuration(actor, cat);
        } else if (ActorStatus_MotionFlags(actor, 0x1200)) {
            ActorStatus_SetRecoveryDuration(actor, cat);
        } else {
            ActorStatus_ClearMotionFlags(actor, 0x400);
            group->common.gate.flags = 0;
            group->turn.turnsRemaining = 0xFF;
            group->turn.direction = 0;
            group->turn.savedDirection = 0;
        }
    }
}

static inline u8 ActorStatus_TryRecoveryRoute(struct ChaseActor *actor, void *route,
                                              struct ContactPosition *from,
                                              struct ContactPosition *to)
{
    s32 blocked;
    if (!Nav_IsPositionInMesh(route, from, to)) {
        ActorStatus_ClearMotionFlags(actor, 0x200);
        return 0;
    }
    blocked = Terrain_ApplyBlocked(route, from, to);
    if (blocked) {
        if (blocked != 2)
            Memory_Clear(&actor->base.body.motion, sizeof(actor->base.body.motion));
        return 1;
    }
    ActorStatus_ClearMotionFlags(actor, 0x200);
    return 0;
}

static inline void ActorStatus_RecoverAndRelease(void *object)
{
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    u32 cat;
    s32 t;
    u32 res;
    struct ContactPosition targetPosition;
    if ((ActorStatus_MotionFlags(object, 0x80)) && !(ActorStatus_MotionFlags(object, 0xA03F)) &&
        ((struct ChaseActor *)object)->base.colliderA.responseValue == 0) {
        if (((struct ChaseActor *)object)->group.common.gate.counter != 0) {
            ((struct ChaseActor *)object)->group.common.gate.counter--;
        } else if ((cat = ((struct ChaseActor *)object)->base.category) != 2 && cat != 0x19 &&
                   !(ActorStatus_MotionFlags(object, 0x100))) {
            u8 *grp;
            struct ContactPosition *position;

            ActorStatus_ResetReaction(object);
            SceneActorAttachment_ReplaceResponseSlot(object, 0, 2, cat);
            SceneActorAnimation_SelectDirectionalEntry(object, 0);
            SceneActorChase_SetControl(object, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
            SceneActorChase_SetState(object, 3, 1, 0);
            SceneActorDelay_Prepare((void *)&actor->group.saved, actor->base.onAlternateEntry, 7, 60);
            ActorStatus_SetWord(&((struct ChaseActor *)object)->base.behaviorFlags, 0x10);
            SceneActorPlacement_ClearFlags(&((struct ChaseActor *)object)->base.reactionFlags,
                                           (u32)(s16)0xF636);

            SceneActorChase_SnapToRoute(&targetPosition,
                                        (void *)&((struct ChaseActor *)object)->group.homeRoute);

            group->destination->pad = 1;
            grp = group->route;
            position = &actor->base.body.position;
            res = ActorStatus_TryRecoveryRoute(object, grp, position, &targetPosition);
            if (!res)
                Callback_InvokeActor(actor, group->fallback);
        }
    }
    if ((ActorStatus_MotionFlags(object, 0x1000)) && !TetherActor_CanStartInteraction())
        Callback_InvokeActor(object, ((struct ChaseActor *)object)->group.interactionCallback);
}

s32 SceneActor_UpdateStatus(struct ChaseActor *actor)
{
    struct ChaseActor *arg0 = (void *)actor;
    if (ActorStatus_StatusFlags(arg0, 0x2000))
        arg0->base.field2C2 = 0x15;
    ActorStatus_TickRouteControl(&arg0->group.common.control);
    if (!ActorStatus_IsDown(arg0))
        ActorStatus_TickAttachment((void *)actor);
    ActorStatus_CollisionGate(actor);
    ActorStatus_EffectSelector((void *)arg0);
    ActorStatus_Palette((void *)arg0);
    ActorStatus_Wake((void *)actor);
    ActorStatus_Blink((void *)arg0);
    ActorStatus_ReactionFlags(actor);
    ActorStatus_Particles((void *)arg0);
    if (!ActorStatus_IsDown(arg0)) {
        ActorStatus_RecoverAndRelease(actor);
        ActorStatus_FaceTarget((void *)arg0);
    }
    ActorStatus_StoreByte(&arg0->base.colliderB.direction, arg0->base.body.direction);
    ActorStatus_StoreByte(&arg0->base.colliderA.direction, arg0->base.body.direction);
    return 1;
}
