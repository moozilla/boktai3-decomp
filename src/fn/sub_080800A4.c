#include "global.h"
#include "scene_actor_chase.h"
#include "engine.h"
#include "scene_actor_delayed_response.h"

static inline void Reaction_ClearWord(u32 *field, u32 bits)
{
    u32 mask = ~bits;
    *field &= mask;
}
static inline void Reaction_ClearHalf(struct ChaseFlags40 *field, u32 bits)
{
    s32 mask = ~bits;
    field->bits &= mask;
}
static inline void Reaction_SetActorFlags(struct ChaseActor *actor, u32 mask)
{
    actor->base.flags2CC = mask | actor->base.flags2CC;
}
static inline void Reaction_Show(struct ChaseActor *actor)
{
    if (!actor->base.drawableKind) {
        u32 *flags = actor->base.drawable;
        s32 mask = ~1;
        *flags = mask & *flags;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile;
        s32 mask = ~1;
        tile->flags = mask & tile->flags;
    }
}
static inline void Reaction_SetDrawMode(struct ChaseActor *actor, u32 mode)
{
    if (!actor->base.drawableKind)
        *((u8 *)actor->base.drawable + 7) = mode;
    else
        *((u8 *)actor->base.drawable + 0x3A) = mode;
}
static inline void SetDeathFlag(struct ChaseActor *actor, u16 bit)
{
    actor->base.flags2D4 = bit | actor->base.flags2D4;
}
static inline void ClearActorBits(struct ChaseActor *actor, u32 bits)
{
    u32 mask = ~bits;
    actor->base.flags2CC &= mask;
}
static inline u32 AnyMotion(u32 mask, u32 *flags)
{
    return mask & *flags;
}
static inline void ApplyColliderMask(u16 *field, u16 bits)
{
    u32 old = *field;
    u32 mask = bits;
    mask &= old;
    *field = mask;
}
static inline u32 *Reaction_ControlFlags(struct ChaseActor *actor)
{
    return &actor->base.flags2CC;
}
void SceneActorReaction_UpdateContactRouteRecovery(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactCollider *collider = &actor->base.colliderA;
    u8 prepared = SceneActorReaction_TakePrepared(actor);
    u32 *motion = &actor->base.flags2D0;

    if (prepared) {
        {
            struct SceneActorRegistryControl *control = &group->common.control;
            Reaction_ClearWord(Reaction_ControlFlags(actor), 0x141C0);
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
        }
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 3, 1);
        collider->flags |= 4;
        collider->responseValue = 0;
        Reaction_Show(actor);
        actor->base.fieldAA = 0;
        Reaction_SetDrawMode(actor, 2);
        ActorShadow_Enable(actor->base.shadow);
        Reaction_ClearWord(&actor->base.flags2D0, 0x80000);
        Reaction_ClearHalf((void *)&actor->base.flags2C0, 0x20);
        ClearActorBits(actor, 0x400000);
        motion = &actor->base.flags2D0;
    }
    if (AnyMotion(0x10, motion)) {
        if (actor->base.health <= 1) {
            s32 zero = 0;
            actor->base.health = zero;
            SetDeathFlag(actor, 8);
            Chase_SetCtl(actor, actor->base.onDefeat, 0x11);
            Chase_SetState(actor, 3, 0, 0);
        } else {
            struct ContactPosition *destination;
            Reaction_SetActorFlags(actor, 0x400000);
            Reaction_ClearWord(motion, 4);
            ApplyColliderMask(&collider->flags, (u16)~4);
            destination = group->destination;
            Chase_SnapToRoute(destination, (u8 *)&group->homeRoute);
            if (!Chase_TryRoute(actor, group->route, &actor->base.body.position,
                                group->destination)) {
                Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
                Chase_SetState(actor, 3, 0, 0);
            } else {
                group->destination->pad = 1;
                Chase_SetState(actor, 3, 1, 0);
                Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
                Reaction_SetActorFlags(actor, 0x10);
                actor->base.field2C2 = 0x40;
                SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
            }
        }
    }
}
