#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"
#define SceneActorControl_RestartOrFinishPhase sub_0808A538
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
inline static u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void SceneActorControl_RestartOrFinishPhase(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    if (TakeFlag(&actor->field2B1)) {
        u32 saved = *(u32 *)((u8 *)actor + 0x224);
        u8 two = 2;
        u8 zero;
        {
            u8 *flagAddr = &actor->field2B2;
            zero = 0;
            *flagAddr = 1;
        }
        actor->field2B0 = zero;
        actor->field2B5 = two;
        actor->target = (void *)saved;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
    }
    {
        void *fn = *(void **)((u8 *)actor + 0x270);
        if (!(u8)sub_0824923C(actor, fn)) {
            if (!(u8)sub_0812F3CC(actor)) {
                u32 offsetA = 0x2A8;
                u32 offsetB = 0x2B0;
                if (*((u8 *)actor + offsetB)) {
                    u8 behaviorVal = 5;
                    u8 zero;
                    void *savedTarget;
                    actor->state2A8 = 0;
                    offsetB -= 7;
                    zero = 0;
                    *((u8 *)actor + offsetB) = zero;
                    offsetA += 2;
                    *((u8 *)actor + offsetA) = behaviorVal;
                    offsetB += 3;
                    *((u32 *)((u8 *)actor + offsetB)) = zero;
                    actor->field2B1 = 1;
                    offsetA -= 0x7E;
                    savedTarget = *(void **)((u8 *)actor + offsetA);
                    offsetB += 6;
                    *((u8 *)actor + offsetB) = 1;
                    actor->field2B0 = zero;
                    offsetB += 3;
                    *((u8 *)actor + offsetB) = zero;
                    {
                        u32 offsetC = 0x2B0;
                        offsetC -= 0x94;
                        *(void **)((u8 *)actor + offsetC) = savedTarget;
                    }
                    actor->field2C8 = zero;
                    {
                        u32 *flags = &actor->flags114;
                        s32 mask = -2;
                        *flags &= mask;
                    }
                    actor->word104 = zero;
                } else {
                    actor->counter2AC++;
                }
            }
        }
    }
}
