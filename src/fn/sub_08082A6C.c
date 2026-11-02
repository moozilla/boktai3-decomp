#include "scene_actor_control_phase.h"
#include "player.h"
s32 Mod(s32, s32);
static inline u8 Tick(void)
{
    return gPlayers[0]->facingChangeEffects;
}
static inline s32 SceneActorControl_TicksSince(s32 now, s32 start)
{
    s32 elapsed;
    if (now < start) {
        s32 wrap = -256;
        {
            s32 adjusted = start + wrap;
            elapsed = now - adjusted;
        }
    } else
        elapsed = now - start;
    return elapsed;
}

void SceneActorControl_UpdateTimedFacing(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    s32 elapsed;
    s32 now;
    s32 start;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        control->startTick = Tick();
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        {
            u32 direction = actor->body.direction;
            {
                u8 *field = &control->savedDirection;
                *field = direction;
            }
        }
    }
    if (!Mod(actor->field2CA, control->pulseInterval)) {
        struct ContactCollider *collider = &actor->colliderB;
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
            u8 *field = &actor->field2B0;
            *field = 1;
        } else {
            u8 *saved = &control->savedDirection;
            u8 *direction;
            direction = &actor->body.direction;
            {
                u32 prior = *saved;
                u32 current;
                current = *direction;
                if (prior != current) {
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                    *saved = *direction;
                } else {

                    u8 *field = &actor->field2B3;
                    *field = 0;
                }
            }
        }
    }
}
