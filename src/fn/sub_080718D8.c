#include "scene_actor_terrain_reaction.h"
#include "scene_actor_reset.h"
void SceneActorControl_OnIdle(u8 *);
void SceneActorPalette_ClearCompletedTransitions(void);
/* The latter two arguments are observed but unused by this initializer. */
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void *ActorAttachment_ReleaseResource(void *, u32, u32);
void *ActorAttachment_ReleasePartsResource(void *, u32, u32);
extern const u8 gUnk_0824E0D8[];
static inline u8 SceneActorRegistry_HasNoObject(struct ChaseActor *actor)
{
    if (!actor->base.object)
        return 1;
    return 0;
}
void SceneActorRegistry_RestoreActorDefaults(struct SceneActorRegistryActor *argument)
{
    struct ChaseActor *actor = (void *)argument;
    u32 *flags;
    u32 responseIndex = 0x3DB;
    SceneActorRegistry_ResetRouteAndColliders((void *)actor);
    SceneActorTerrain_EnterState(actor, 3, 0, 0);
    SceneActorTerrain_InstallControl(actor, SceneActorControl_OnIdle, 16);
    if (SceneActorRegistry_HasNoObject(actor)) {
        s32 health;
        actor->base.flags2CC = actor->base.saved.behaviorFlags;
        actor->base.flags2D0 = actor->base.saved.statusFlags;
        actor->base.body.direction = actor->base.saved.direction;
        actor->base.state2A8 = actor->base.saved.state;
        actor->base.phase2A9 = actor->base.saved.phase;
        actor->base.behavior2AA = actor->base.saved.behavior;
        actor->base.target = (void *)actor->base.saved.control;
        actor->base.field2B5 = actor->base.saved.kind;
        actor->base.flags2C0 = actor->base.saved.motionFlags;
        actor->base.field2EA = actor->base.saved.heightOffset;
        health = actor->base.defaultHealth;
        flags = &actor->base.flags2CC;
        if (health <= 0)
            health = 0;
        actor->base.health = health;
        actor->base.flags2D6 = 0;
        actor->base.flags2D4 = 0;
        actor->base.field2B0 = 0;
        actor->base.field2B1 = 1;
        actor->base.field2B2 = 1;
        actor->base.restart = 0;
        actor->base.field304 = 0;
        actor->base.field2B6 = 0;
        actor->base.field2C2 = 64;
        actor->base.field2D8 = 0;
        actor->base.field2DC = 0;
        actor->base.field2E0 = 0;
        actor->base.field2E6 = 0;
        actor->base.field3CC = 0;
        actor->base.tracking.target = 0;
        SceneActorPalette_ClearCompletedTransitions();
    } else {
        s32 health;
        actor->base.flags2CC = actor->base.saved.behaviorFlags;
        actor->base.flags2D0 = actor->base.saved.statusFlags;
        actor->base.body.direction = actor->base.saved.direction;
        actor->base.state2A8 = actor->base.saved.state;
        actor->base.phase2A9 = actor->base.saved.phase;
        actor->base.behavior2AA = actor->base.saved.behavior;
        actor->base.target = (void *)actor->base.saved.control;
        actor->base.field2B5 = actor->base.saved.kind;
        actor->base.flags2C0 = actor->base.saved.motionFlags;
        actor->base.field2EA = actor->base.saved.heightOffset;
        health = actor->base.defaultHealth;
        flags = &actor->base.flags2CC;
        if (health <= 0)
            health = 0;
        actor->base.health = health;
        actor->base.flags2D6 = 0;
        actor->base.flags2D4 = 0;
        actor->base.field2B0 = 0;
        actor->base.field2B1 = 1;
        actor->base.field2B2 = 1;
        actor->base.restart = 0;
        actor->group.common.gate.flags = 0;
        actor->group.common.gate.counter = 0;
        actor->base.fieldAA = 0;
        actor->base.field304 = 0;
        actor->base.field2B6 = 0;
        actor->base.field2C2 = 64;
        actor->base.field2D8 = 0;
        actor->base.field2DC = 0;
        actor->base.field2E0 = 0;
        actor->base.field2E6 = 0;
        actor->base.field3CC = 0;
        actor->base.tracking.target = 0;
        actor->base.field2B8 = 0;
        actor->group.hitReactionLatch = 0;
        actor->group.field5C6 = 0;
        actor->group.common.control.mode = 0;
        Memory_Clear(&actor->group.motion5B8, 8);
        actor->group.field5C4 = 0;
        actor->group.field5C0 = 0;
        actor->group.field5C2 = 0;
        SceneActorReaction_ResetResponseState(&actor->group.saved, gUnk_0824E0D8, responseIndex);
        actor->group.turn.direction = 0;
        actor->group.turn.savedDirection = 0;
        actor->group.turn.turnsRemaining = -1;
        actor->group.saved.control = 0;
        actor->group.saved.state = 0;
        actor->group.saved.phase = 0;
        actor->group.saved.behavior = 0;
        actor->group.saved.kind = 0;
        SceneActorPalette_ClearCompletedTransitions();
        actor->group.attachment =
            ActorAttachment_ReleaseResource(actor->group.attachment, 1, actor->base.body.id);
        actor->group.attachment =
            ActorAttachment_ReleasePartsResource(actor->group.attachment, 3, actor->base.body.id);
    }
    {
        u32 two = 2;
        *flags |= two;
    }
    {
        struct ContactCollider *collider = &actor->base.colliderA;
        collider->flags |= 4;
    }
    if (actor->base.callback248)
        Callback_InvokeActor(actor, actor->base.callback248);
    SceneActorPalette_ClearCompletedTransitions();
}
