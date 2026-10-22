#include "scene_actor_chase.h"
u8 Skeleton_RerouteToTarget(void *);
u8 SceneActor_DispatchContextAction(void *, u32);
// confidence=low: after the action delay, restores a reaction or reroutes toward the player.
#define Vorn_ResumeRouteAfterDelay sub_080AD530
#ifdef NONMATCHING
// fuzzy: 94.5%
// Remaining differences: flag-load scheduling and derived group-field addresses.
void Vorn_ResumeRouteAfterDelay(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u8 *group = (u8 *)&actor->group;
    u8 *context = actor->context;
    if (actor->field2B2)
        actor->field2B2 = 0;
    {
        u32 mask = 16;
        actor->flags2D6 = mask | actor->flags2D6;
    }
    if (frame <= 18)
        return;
    actor->field2B0 = 1;
    if (Chase_MotionFlags(actor, 2)) {
        u32 mask;
        Chase_SetState(actor, 3, 0, 0);
        Chase_SetCtl(actor, *(void **)(group + 0x208), 7);
        mask = ~0x983;
        actor->flags2D6 = mask & actor->flags2D6;
    } else if (Chase_Test(*(u16 *)(context + 0x6D4), 0x1D))
        SceneActor_DispatchContextAction(context, 1);
    else {
        actor->fieldDC = (u32)gUnk_02000580[0] + 0x24;
        {
            u32 mask = 16;
            actor->flags2CC |= mask;
        }
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        if (!Skeleton_RerouteToTarget(actor) &&
            !Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                            (void *)(actor->fieldDC + 12))) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return;
        }
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 7);
        actor->field2C2 = 32;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080AD530);
#endif
