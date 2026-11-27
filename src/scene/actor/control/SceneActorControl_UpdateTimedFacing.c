#include "scene/actor/scene_actor_control_phase.h"
#include "player/player.h"
s32 Mod(s32, s32);
static inline u8 Tick(void)
{
    return gPlayers[0]->facingChangeEffects;
}
static inline s32 SceneActorControl_TicksSince(s32 now, s32 start)
{
    s32 elapsed;
    if (now < start) {
        {
            s32 adjusted = start + (-256);
            elapsed = now - adjusted;
        }
    } else
        elapsed = now - start;
    return elapsed;
}

void SceneActorControl_UpdateTimedFacing(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    s32 elapsed;
    s32 now;
    s32 start;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        control->startTick = Tick();
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        {
            u32 direction = actor->base.body.direction;
            {
                u8 *field = &control->savedDirection;
                *field = direction;
            }
        }
    }
    if (!Mod(actor->base.frameCounter, control->pulseInterval)) {
        struct ContactCollider *collider = &actor->base.colliderB;
        collider->flags &= 0xFFFB;
    }
    start = control->startTick;
    now = Tick();
    elapsed = SceneActorControl_TicksSince(now, start);
    control->elapsed = elapsed;
    {
        u8 *limitField = &control->maxElapsed;
        u8 limit;
        limit = *limitField;
        if ((u8)elapsed > limit) {
            u8 *field = &actor->base.stepDone;
            *field = 1;
        } else {
            u8 *saved = &control->savedDirection;
            u8 *direction;
            direction = &actor->base.body.direction;
            {
                u32 prior = *saved;
                u32 current;
                current = *direction;
                if (prior != current) {
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                    *saved = *direction;
                } else {

                    u8 *field = &actor->base.restart;
                    *field = 0;
                }
            }
        }
    }
}
