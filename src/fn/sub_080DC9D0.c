#include "global.h"

// confidence=low: checkpoint completion restores player-route control and retries the route.

#include "scene_actor_chase.h"

void SceneActor_UpdateAnimationCheckpoint(void);
void Mummy_UpdatePlayerRoute(void);
void SceneActorControl_OnIdle(void);

struct MummyCheckpointContext {
    u8 unknown0[0x6F2];
    s16 flags6F2 : 16;
};

static inline void Mummy_ClearMotion(struct SceneActorRegistryResetView *actor, u32 bits)
{
    struct ChaseFlags40 *field = (void *)&actor->flags2D4;
    s32 mask = ~bits;
    field->bits &= mask;
}
static inline void Mummy_ClearCheckpointFlags(struct ChaseFlags40 *flags, s32 mask)
{
    flags->bits &= mask;
}

void Mummy_ResumeRouteAfterCheckpoint(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    struct MummyCheckpointContext *body = actor->context;
    if (Chase_TakeArrived(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Chase_SetCtl(actor, SceneActor_UpdateAnimationCheckpoint, 0x26);
    }
    if (actor->field2B0) {
        s32 contextBits = 0x800;
        Mummy_ClearMotion(actor, 0x100);
        body->flags6F2 &= ~contextBits;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Mummy_ClearCheckpointFlags((void *)(group + 0x40), ~6);
        {
            u32 mask = 0x1000;
            body->flags6F2 = mask | body->flags6F2;
        }
        Chase_SetCtl(actor, Mummy_UpdatePlayerRoute, 0x27);
        Chase_SetBehavior(actor, 0x10);
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position, &actor->body.position)) {
            Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
            Chase_SetState(actor, 3, 0, 0);
        }
    }
}
