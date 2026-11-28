#include "scene/actor/scene_actor_control_phase.h"
#include "player/player.h"
s32 Mod(s32, s32);
static inline s32 SceneActorControl_TicksSince(s32 now, s32 start)
{
    if (now < start)
        return now + 256 - start;
    return now - start;
}

void SceneActorControl_UpdateTimedFacing(struct ChaseActor *actor)
{
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    s32 elapsed;
    s32 now;
    s32 start;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        control->startTick = gPlayers[0]->facingChangeEffects;
        SceneActorAnimation_SelectDirectionalEntry((struct RoomActor *)actor, 1);
        control->savedDirection = actor->base.body.direction;
    }
    if (!Mod(actor->base.frameCounter, control->pulseInterval)) {
        struct ContactCollider *collider = &actor->base.colliderB;
        collider->flags &= 0xFFFB;
    }
    start = control->startTick;
    now = gPlayers[0]->facingChangeEffects;
    elapsed = SceneActorControl_TicksSince(now, start);
    control->elapsed = elapsed;
    if ((u8)elapsed > control->maxElapsed)
        actor->base.stepDone = 1;
    else if (control->savedDirection != actor->base.body.direction) {
        SceneActorAnimation_SelectDirectionalEntry((struct RoomActor *)actor, 1);
        control->savedDirection = actor->base.body.direction;
    } else
        actor->base.restart = 0;
}
