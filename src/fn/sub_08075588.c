#include "scene_actor_status.h"

void SceneActorRecord_RecordPair(u32, u32);

static inline void SceneActorMovement_Wake(struct ChaseActor *actor)
{
    /* wake-up transition when the stun flag is set */
    if (!ActorStatus_IsDown(actor) && ActorStatus_BehaviorFlags(actor, 0x1000)) {
        SceneActorChase_ClearWordFlags(&actor->base.flags2CC, 0x1000);
        actor->base.reactionStart.x = actor->base.body.position.x;
        actor->base.reactionStart.z = actor->base.body.position.z;
        actor->base.reactionEnd.x = actor->base.body.position.x;
        actor->base.reactionEnd.z = actor->base.body.position.z;
        if (actor->base.health <= 0) {
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, actor->base.onDefeat, 0x11);
            if (!SceneEnvironment_LinkMode() && !ActorStatus_StatusFlags(actor, 0x800))
                SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->base.body.id);
        } else {
            SceneActorChase_SetControl(actor, actor->base.onReaction, 0x12);
            SceneActorChase_SetState(actor, 3, 0, 0);
        }
    }
}

static inline void SceneActorMovement_ApplyReactionFlags(struct ChaseActor *actor)
{
    /* state-flag dispatch */
    if (actor->base.flags2D4 == 0) {
        SceneActorChase_ClearWordFlags(&actor->base.flags2CC, 0x2000);
    } else if (ActorStatus_IsDown(actor)) {
        actor->base.flags2D4 = 0x40;
        actor->base.field2E6 = 0;
        SceneActorChase_ClearBits(&actor->base.flags2D6, 0x980);
    } else if (ActorStatus_MotionFlags(actor, 0x803E)) {
        ActorStatus_ClearMotionFlags(actor, 0x201);
        SceneActorChase_ClearWordFlags(&actor->base.flags2CC, 0x4000);
        ActorStatus_ClearTrackingFlags((void *)&actor->base.tracking.target, 0x10);
        if (!(ActorStatus_MotionFlags(actor, 0x20)))
            SceneActorChase_ClearWordFlags(&actor->base.flags2CC, 0x80);
        SceneActorChase_ClearWordFlags(&actor->base.flags2D0, 0x100004);
        ActorStatus_SetPendingBits(actor, 0x400);
        actor->base.field2E6 = 0;
        ActorStatus_SetWord(&actor->base.flags2CC, 0x2000);
    } else {
        SceneActorChase_ClearWordFlags(&actor->base.flags2CC, 0x2000);
    }
}

s32 SceneActorMovement_UpdateStatus(struct ChaseActor *object)
{
    struct ChaseActor *actor = (void *)object;
    ActorStatus_Palette(actor);
    SceneActorMovement_Wake(actor);
    ActorStatus_Blink(actor);
    SceneActorMovement_ApplyReactionFlags(actor);
    ActorStatus_Particles(actor);
    if (!ActorStatus_IsDown(actor))
        ActorStatus_FaceTarget(actor);
    ActorStatus_StoreByte(&actor->base.colliderB.direction, actor->base.body.direction);
    ActorStatus_StoreByte(&actor->base.colliderA.direction, actor->base.body.direction);
    return 1;
}
