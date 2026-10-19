#include "global.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 85.4%
// foundry draft (worker best 85.4%); what differs is mostly register allocation and instruction order
#define SceneActorControl_RestartOrAdvance sub_0808A430
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorControl_RestartOrAdvance(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        u32 saved = *(u32 *)((u8 *)actor + 0x224);
        u8 two = 2;
        u8 zero;
        zero = 0;
        actor->field2B2 = 1;
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
        u16 *gate = (void *)((u8 *)control + 0x63C);
        if (*gate == 0) {
            void *fn = *(void **)((u8 *)actor + 0x270);
            u16 *flags;
            if ((u8)Callback_InvokeActor(actor, fn)) {
                flags = (void *)((u8 *)control + 0x628);
            } else {
                if (!sub_0812F3CC(actor))
                    goto fieldCheck;
                flags = (void *)((u8 *)control + 0x628);
            }
            {
                s32 mask = -2;
                *flags &= mask;
            }
            return;
        }
    }
fieldCheck:
    if (actor->field2B0) {
        u8 behaviorVal = 0xF;
        u32 offset = 0x2B0;
        u8 zero;
        actor->state2A8 = 0;
        zero = 0;
        offset -= 7;
        *((u8 *)actor + offset) = zero;
        offset += 1;
        *((u8 *)actor + offset) = behaviorVal;
        actor->counter2AC = zero;
        offset += 7;
        *((u8 *)actor + offset) = 1;
    } else {
        actor->counter2AC++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0808A430);
#endif
