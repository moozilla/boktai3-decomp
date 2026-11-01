#include "scene_actor_status.h"

typedef char Check_attachmentLimit
    [(u32) & ((struct SceneActorRegistryResetView *)0)->attachmentLimit == 0xA6 ? 1 : -1];
typedef char
    Check_attachmentTimer[(u32) & ((struct SceneActorRegistryResetView *)0)->fieldAA == 0xAA ? 1
                                                                                             : -1];
typedef char Check_alternatePalette
    [(u32) & ((struct SceneActorRegistryResetView *)0)->alternatePalette == 0x1DC ? 1 : -1];
typedef char
    Check_flashTimer[(u32) & ((struct SceneActorRegistryResetView *)0)->field1E8 == 0x1E8 ? 1 : -1];
typedef char Check_deathControl
    [(u32) & ((struct SceneActorRegistryResetView *)0)->deathControl == 0x234 ? 1 : -1];
typedef char Check_secondaryMotion
    [(u32) & ((struct SceneActorRegistryResetView *)0)->secondaryMotion == 0x2EC ? 1 : -1];
typedef char
    Check_particleTimer[(u32) & ((struct SceneActorRegistryResetView *)0)->field30C == 0x30C ? 1
                                                                                             : -1];
typedef char Check_route[(u32) & ((struct SceneActorRegistryResetView *)0)->group.homeRoute == 0x448
                             ? 1
                             : -1];
typedef char Check_contactNode
    [(u32) & ((struct SceneActorRegistryResetView *)0)->group.contactNode == 0x45C ? 1 : -1];
typedef char Check_countdown5C8
    [(u32) & ((struct SceneActorRegistryResetView *)0)->group.countdown5C8 == 0x5C8 ? 1 : -1];
typedef char
    Check_effect5CC[(u32) & ((struct SceneActorRegistryResetView *)0)->group.effect == 0x5CC ? 1
                                                                                             : -1];
typedef char Check_behaviorControl
    [(u32) & ((struct SceneActorRegistryResetView *)0)->group.contactControl == 0x5DC ? 1 : -1];
typedef char Check_interactionCallback
    [(u32) & ((struct SceneActorRegistryResetView *)0)->group.interactionCallback == 0x618 ? 1
                                                                                           : -1];
typedef char Check_responseCallback
    [(u32) & ((struct SceneActorRegistryResetView *)0)->group.responseCallback == 0x61C ? 1 : -1];
struct UpdateRestorationCommand {
    u8 unknown0[8];
    u8 state, phase, behavior, unknownB;
    u32 delay;
    void *control;
};
static inline void ActorStatus_StoreRestoration(struct UpdateRestorationCommand *command,
                                                void *control, u32 state, u32 phase, u32 behavior,
                                                u32 delay)
{
    command->delay = delay;
    command->control = control;
    command->state = state;
    command->phase = phase;
    command->behavior = behavior;
}

static inline void ActorStatus_MaskHalfword(u16 *field, u32 mask)
{
    *field &= mask;
}

void SceneActorRecord_RecordPair(u32, u32);
void SceneActorAttachment_ReplaceResponseSlot(void *, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);
void SceneActorContacts_ReleaseNode(void *);
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



static inline void ActorStatus_ReleaseContact(struct SceneActorRegistryResetView *actor)
{
    u32 flag = 0x1000;
    if (actor->flags2D4 & flag) {
        SceneActorContacts_ReleaseNode(actor->group.contactNode);
        actor->flags2D4 &= ~flag;
        actor->group.contactNode = 0;
    }
}

static inline void ActorStatus_ResetInteractionAt(struct SceneActorRegistryResetView *actor,
                                                  struct SceneActorRegistryControl *control)
{
    if (ActorStatus_HasBehaviorFlags(actor, 0x200000)) {
        ActorStatus_ClearInteraction(actor, control);
    }
}

static inline void ActorStatus_ResetInteraction(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    ActorStatus_ResetInteractionAt(actor, (void *)&actor->group.common.control);
}
static inline void ActorStatus_ResetRouteAt(struct SceneActorRegistryResetView *actor,
                                            struct SceneActorRegistryControl *control)
{
    SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x141C0);
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}
static inline void ActorStatus_ResetRoute(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    ActorStatus_ResetRouteAt(actor, (void *)&actor->group.common.control);
}
static inline void ActorStatus_ResetSelection(struct SceneActorRegistryResetView *actor)
{
    actor->group.turn.turnsRemaining = 255;
}

/* shared reset of the knockback/stun scratch state */
static inline void ActorStatus_ResetReaction(struct SceneActorRegistryResetView *actor)
{
    actor->field2C2 = 0x40;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    ActorStatus_ClearMotionFlags(actor, 0x380);
    ActorStatus_ClearTrackingFlags((void *)&actor->tracking.target, 0x18);
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
static inline void ActorStatus_TickAttachment(struct SceneActorRegistryResetView *actor)
{
    s32 remaining = (*(s16 *)&actor->fieldAA);
    if (remaining <= 0) {
        (*(s16 *)&actor->fieldAA) = 0;
        if (ActorStatus_HasStatusFlags(actor, 0xC0))
            ActorStatus_SetWord(&actor->flags2D0, 0x100);
    } else {
        (*(s16 *)&actor->fieldAA) = remaining - 1;
    }
    if (actor->group.countdown5C8)
        actor->group.countdown5C8--;
}

static inline u32 ActorStatus_IsUnstable(struct SceneActorRegistryResetView *actor)
{
    u32 result = 0;
    if (ActorStatus_BodyFlags(actor, 0x1000))
        return 1;
    if (ActorStatus_MotionFlags(actor, 0xA03F) || actor->colliderA.responseValue != 0 ||
        (*(s16 *)&actor->field2EA) > 0 || (*(u32 *)&actor->secondaryMotion.x) != 0 ||
        actor->secondaryMotion.z != 0)
        return 1;
    if (ActorStatus_IsDown(actor))
        result = 1;
    return result;
}
static inline void ActorStatus_CollisionGate(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
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

static inline void ActorStatus_EffectSelector(struct SceneActorRegistryResetView *actor)
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

static inline s16 ActorStatus_AttachmentRestart(struct SceneActorRegistryResetView *actor)
{
    return actor->fieldA8;
}

static inline void ActorStatus_Wake(struct SceneActorRegistryResetView *actor)
{
    const struct SceneActorRegistryActor *identity = (void *)actor;

    /* wake-up transition when the stun flag is set */
    if (!(ActorStatus_MotionFlags(actor, 0x40)) && !(ActorStatus_StatusFlags(actor, 0x20000)) &&
        (ActorStatus_BehaviorFlags(actor, 0x1000))) {
        ActorStatus_ResetRoute(actor);
        SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x1000);
        actor->reactionStart.x = actor->body.position.x;
        actor->reactionStart.z = actor->body.position.z;
        actor->reactionEnd.x = actor->body.position.x;
        actor->reactionEnd.z = actor->body.position.z;
        if ((*(s16 *)&actor->fieldAA) >= actor->attachmentLimit)
            (*(s16 *)&actor->fieldAA) = ActorStatus_AttachmentRestart(actor);
        if (actor->health <= 0) {
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, actor->deathControl, 0x11);
            if (!SceneEnvironment_LinkMode() && !ActorStatus_StatusFlags((void *)identity, 0x800))
                SceneActorRecord_RecordPair(
                    gScriptEntryProcedure,
                    (u16)((const struct SceneActorRegistryActor *)identity)->id);
        } else if (ActorStatus_BehaviorFlags(actor, 0x200)) {
            SceneActorChase_SetControl(actor, actor->reactionTarget230, 0x12);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else {
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0x7F, actor->category);
            Callback_InvokeActor(actor, actor->group.responseCallback);
            SceneActorChase_SetControl(actor, actor->group.contactControl, 7);
            SceneActorChase_SetState(actor, 3, 0, 0);
        }
    }
}

static inline void ActorStatus_SetRecoveryDuration(struct SceneActorRegistryResetView *actor,
                                                   u32 category)
{
    if (category == 0x15 || category == 0x19) {
        actor->field2C2 = 0x20;
        return;
    }
    if (category - 5 <= 2) {
        actor->field2C2 = 0x20;
        return;
    }
    actor->field2C2 = Div(0x40, 3);
}

static inline void ActorStatus_ReactionFlags(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    u32 cat;
    /* state-flag dispatch */
    if (((struct SceneActorRegistryResetView *)object)->flags2D4 == 0) {
        SceneActorChase_ClearWordFlags(&((struct SceneActorRegistryResetView *)object)->flags2CC,
                                       0x2000);
    } else if (ActorStatus_IsDown(object)) {
        ActorStatus_ReleaseContact(object);
        ActorStatus_ResetInteractionAt(actor, (void *)((u8 *)group + 0x4C));
        actor->flags2D4 = 0x40;
        actor->field2E6 = 0;
        SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
    } else {

        cat = actor->category;

        if (ActorStatus_MotionFlags(actor, 0x2000)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->tracking.target, 8);
            ActorStatus_ClearMotionFlags(actor, 0x500);
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->field2C2 = 0x40;
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 4)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->tracking.target, 8);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->field2C2 = 0x40;
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
            ActorStatus_SetWord(&actor->flags2CC, 0x2000);
        } else if (ActorStatus_MotionFlags(actor, 2)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->tracking.target, 8);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 0x8000)) {
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 0x10)) {
            ActorStatus_ResetRouteAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            (*(s16 *)&actor->fieldAA) = 0;
            actor->field2C2 = Div(0x40, 6);
            ActorStatus_ReleaseContact(object);
            ActorStatus_ResetInteractionAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ClearMotionFlags(actor, 0x400);
            group->turn.turnsRemaining |= -1;
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 8)) {
            ActorStatus_ResetRouteAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ResetInteractionAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        }

        if (ActorStatus_MotionFlags(actor, 0x803E)) {
            ActorStatus_ClearMotionFlags(actor, 0x201);
            SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x4000);
            ActorStatus_ClearTrackingFlags((void *)&actor->tracking.target, 0x10);
            if (!(ActorStatus_MotionFlags(actor, 0x20)))
                SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x80);
            SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x100004);
            ActorStatus_ReleaseContact(object);
            ActorStatus_ResetInteractionAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ClearMotionFlags(actor, 0x400);
            group->turn.turnsRemaining = 0xFF;
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->field2E6 = 0;
            if (!(SceneActorChase_ReadMotionFlags(actor) & 0x10))
                ActorStatus_SetWord(&actor->flags2CC, 0x2000);
            else
                SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x2000);
        } else {
            SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x2000);
        }

        if (ActorStatus_MotionFlags(actor, 0x400)) {
            if (group->turn.turnsRemaining < 0 ||
                (actor->field2B5 != 0x1B && actor->field2B5 != 2)) {
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

static inline u8 ActorStatus_TryRecoveryRoute(struct SceneActorRegistryResetView *actor,
                                              void *route, struct ContactPosition *from,
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
            Memory_Clear(&actor->body.motion, sizeof(actor->body.motion));
        return 1;
    }
    ActorStatus_ClearMotionFlags(actor, 0x200);
    return 0;
}

static inline void ActorStatus_RecoverAndRelease(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    u32 cat;
    s32 t;
    u32 res;
    struct ContactPosition targetPosition;
    if ((ActorStatus_MotionFlags(object, 0x80)) && !(ActorStatus_MotionFlags(object, 0xA03F)) &&
        ((struct SceneActorRegistryResetView *)object)->colliderA.responseValue == 0) {
        if (((struct SceneActorRegistryResetView *)object)->group.common.gate.counter != 0) {
            ((struct SceneActorRegistryResetView *)object)->group.common.gate.counter--;
        } else if ((cat = ((struct SceneActorRegistryResetView *)object)->category) != 2 &&
                   cat != 0x19 && !(ActorStatus_MotionFlags(object, 0x100))) {
            u8 *grp;
            u8 *r5;

            ActorStatus_ResetReaction(object);
            SceneActorAttachment_ReplaceResponseSlot(object, 0, 2, cat);
            SceneActorAnimation_SelectDirectionalEntry(object, 0);
            SceneActorChase_SetControl(object, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
            SceneActorChase_SetState(object, 3, 1, 0);
            ActorStatus_StoreRestoration(
                (void *)&((struct SceneActorRegistryResetView *)object)->group.saved.control,
                ((struct SceneActorRegistryResetView *)object)->entryTarget228, 0, 0, 7, 60);
            ActorStatus_SetWord(&((struct SceneActorRegistryResetView *)object)->flags2CC, 0x10);
            ActorStatus_MaskHalfword(&((struct SceneActorRegistryResetView *)object)->flags2D6,
                                     (u32)(s16)0xF636);

            SceneActorChase_SnapToRoute(
                &targetPosition,
                (void *)&((struct SceneActorRegistryResetView *)object)->group.homeRoute);

            group->destination->pad = 1;
            grp = group->route;
            r5 = (u8 *)actor + 0x5C;
            res = ActorStatus_TryRecoveryRoute(object, grp, (void *)r5, &targetPosition);
            if (!res)
                Callback_InvokeActor(actor, group->fallback);
        }
    }
    if ((ActorStatus_MotionFlags(object, 0x1000)) && !TetherActor_CanStartInteraction())
        Callback_InvokeActor(
            object, ((struct SceneActorRegistryResetView *)object)->group.interactionCallback);
}

s32 SceneActor_UpdateStatus(struct SceneActorRegistryActor *actor)
{
    struct SceneActorRegistryActor *arg0 = (void *)actor;
    if (ActorStatus_StatusFlags(arg0, 0x2000))
        ((struct SceneActorRegistryResetView *)arg0)->field2C2 = 0x15;
    ActorStatus_TickRouteControl(
        &((struct SceneActorRegistryResetView *)arg0)->group.common.control);
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
    ActorStatus_StoreByte(&((struct SceneActorRegistryResetView *)arg0)->colliderB.direction,
                          ((struct SceneActorRegistryResetView *)arg0)->body.direction);
    ActorStatus_StoreByte(&((struct SceneActorRegistryResetView *)arg0)->colliderA.direction,
                          ((struct SceneActorRegistryResetView *)arg0)->body.direction);
    return 1;
}
