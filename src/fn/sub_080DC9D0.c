#include "global.h"

// confidence=low: checkpoint completion restores player-route control and retries the route.

#ifdef NONMATCHING
// fuzzy: 99.3%
// Three constant/load ordering differences remain; control flow and registers agree.
#include "scene_actor_chase.h"

void SceneActor_UpdateAnimationCheckpoint(void);
void Mummy_UpdatePlayerRoute(void);
void SceneActorControl_OnIdle(void);

struct MummyCheckpointContext {
    u8 unknown0[0x6F2];
    s16 flags6F2 : 16;
};

void Mummy_ResumeRouteAfterCheckpoint(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    struct MummyCheckpointContext *body = actor->context;
    if (Chase_TakeArrived(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Chase_SetCtl(actor, SceneActor_UpdateAnimationCheckpoint, 0x26);
    }
    if (actor->field2B0) {
        s32 contextBits = 0x800;
        Chase_ClearMotionFlags(actor, 0x100);
        body->flags6F2 &= ~contextBits;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        ((struct ChaseFlags40 *)(group + 0x40))->bits &= ~6;
        {
            u32 mask = 0x1000;
            body->flags6F2 = mask | body->flags6F2;
        }
        Chase_SetCtl(actor, Mummy_UpdatePlayerRoute, 0x27);
        Chase_SetBehavior(actor, 0x10);
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position, &actor->position)) {
            Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
            Chase_SetState(actor, 3, 0, 0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Mummy_ResumeRouteAfterCheckpoint);
#endif
