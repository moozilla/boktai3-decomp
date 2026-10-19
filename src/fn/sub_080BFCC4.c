#include "global.h"
#ifdef NONMATCHING
// fuzzy: 53.5%
// Idle/chase state built on SceneActorChase_Pursue (include/scene_actor_chase.h). The
// counter2AC <= 60 increment is the shared tail at 080C0644. The not-chasing branch inlines a
// second move-toward step (flags2D4 &= ~0x200 via bics with the 0x200 offset constant).
#include "scene_actor_chase.h"

u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void sub_0807C72C(struct SceneActorRegistryResetView *, u32, u32, u32);

void sub_080BFCC4(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;

    if (SceneActorChase_TakeFlag2B1(actor))
        actor->direction = actor->fieldE9;
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    if (sub_0812F3CC(actor))
        return;
    if ((s32)actor->counter2AC > 0x3C) {
        if (actor->flags2D4 & 0x80) {
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = 0xC;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            SceneActorChase_SetControl(actor, 0x1A, *(void **)(group + 0x48));
            SceneActorChase_Pursue(actor);
        } else {
            struct ContactPosition *dest;
            void *mover;
            s32 result;

            actor->state2A8 = 3;
            actor->phase2A9 = 1;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            SceneActorChase_SetControl(actor, 0xE, SceneActorReaction_RestoreAfterDelay);
            sub_0807C72C(actor, 0, 2, actor->category);
            {
                u8 *saved = group + 0x8C;
                *(u32 *)(saved + 0xC) = 0x1E;
                *(void **)(saved + 0x10) = actor->entryTarget228;
                saved[8] = 0;
                saved[9] = 0;
                saved[10] = 7;
            }
            dest = *(struct ContactPosition **)(group + 0x200);
            {
                struct SceneActorChaseRouteLink *link =
                    (struct SceneActorChaseRouteLink *)(group + 0x74);
                dest->x = link->point->x;
                dest->z = link->point->z;
                dest->y = SceneActorChase_CellHeight(dest, SceneActorChase_CellIndex(dest));
            }
            mover = *(void **)(group + 0x1FC);
            dest = *(struct ContactPosition **)(group + 0x200);
            if (!sub_0821F204(mover, &actor->position, dest)) {
                actor->flags2D4 &= ~0x200;
                result = 0;
            } else {
                result = sub_0821F7A8(mover, &actor->position, dest);
                if (result == 0) {
                    actor->flags2D4 &= ~0x200;
                    result = 0;
                } else {
                    if (result != 2)
                        Memory_Clear(&actor->motion, 8);
                    result = 1;
                }
            }
            if (result == 0) {
                SceneActorChase_SetControl(actor, 0, SceneActorControl_OnIdle);
                actor->state2A8 = 3;
                actor->phase2A9 = 0;
                actor->behavior2AA = 0;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            } else {
                (*(struct ContactPosition **)(group + 0x200))->pad = 1;
            }
        }
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BFCC4);
#endif
