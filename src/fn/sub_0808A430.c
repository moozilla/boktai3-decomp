// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: restarts saved control or advances behavior after callbacks complete.
#include "scene_actor_control_phase.h"
#include "engine.h"
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorControl_RestartOrAdvance(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        register u32 offset asm("r3") = 0x224;
        u32 saved;
        register u32 *slot asm("r0");
        u32 two;
        u8 zero;
        u8 *q2;
        asm("" : "+r"(offset));
        slot = (void *)((u8 *)actor + offset);
        saved = *slot;
        two = 2;
        q2 = &actor->field2B2;
        zero = 0;
        *q2 = 1;
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
            register u32 offset asm("r3") = 0x270;
            void *fn;
            register void **slot asm("r0");
            register u16 *flags asm("r0");
            asm("" : "+r"(offset));
            slot = (void *)((u8 *)actor + offset);
            fn = *slot;
            if ((u8)Callback_InvokeActor(actor, fn)) {
                {
                    u32 offset = 0x628;
                    flags = (void *)((u8 *)control + offset);
                }
            } else {
                if (!sub_0812F3CC(actor))
                    goto fieldCheck;
                {
                    u32 offset = 0x628;
                    flags = (void *)((u8 *)control + offset);
                }
            }
            {
                s32 mask = -2;
                *flags &= mask;
            }
            return;
        }
    }
fieldCheck: {
    u32 offset = 0x2B0;
    asm("" : "+r"(offset));
    if (*((u8 *)actor + offset)) {
        u8 behaviorVal = 0xF;
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
}
