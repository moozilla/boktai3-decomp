#define SceneActorControl_UpdateSavedControlPhase sub_0813067C
#include "global.h"
#include "scene_actor_reset.h"
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void sub_0813067C(struct SceneActorRegistryResetView *actor)
{
    if (TakeFlag(&actor->field2B1)) {
        void *callback = actor->savedTarget;
        u32 zero = 0;
        actor->field2B2 = 1;
        actor->field2B0 = zero;
        actor->field2B5 = zero;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
    } else {
        if (!actor->counter2AC)
            Trigger_ConsumePending((u8 *)actor + 0xC, 1);
        ++actor->counter2AC;
    }
}
