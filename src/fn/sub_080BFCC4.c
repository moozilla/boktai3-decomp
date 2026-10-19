#include "global.h"
#ifdef NONMATCHING
// fuzzy: 75.6%
// Idle/chase state built on the shared chase helpers (include/scene_actor_chase.h, taken
// from the sub_080C09F0 draft). The counter2AC <= 60 increment is the shared tail at 080C0644.
#include "scene_actor_chase.h"

u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void sub_0807C72C(struct SceneActorRegistryResetView *, u32, u32, u32);

void sub_080BFCC4(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;

    if (Chase_TakeArrived(actor))
        actor->direction = actor->fieldE9;
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    if (sub_0812F3CC(actor))
        return;
    if ((s32)actor->counter2AC > 0x3C) {
        if (Chase_MotionFlags(actor, 0x80)) {
            Chase_SetBehavior(actor, 12);
            Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
            Chase_Arrive(actor);
        } else {
            Chase_SetState(actor, 3, 1, 0);
            Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
            sub_0807C72C(actor, 0, 2, actor->category);
            {
                u8 *saved = group + 0x8C;
                *(u32 *)(saved + 0xC) = 0x1E;
                *(void **)(saved + 0x10) = actor->entryTarget228;
                saved[8] = 0;
                saved[9] = 0;
                saved[10] = 7;
            }
            Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
            if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                *(struct ContactPosition **)(group + 0x200))) {
                Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 0);
                Chase_SetState(actor, 3, 0, 0);
            } else
                (*(struct ContactPosition **)(group + 0x200))->pad = 1;
        }
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BFCC4);
#endif
