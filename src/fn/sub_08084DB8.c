#include "scene_actor_control_helpers.h"

void SceneActorControl_ConsumePendingEntry(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *fn = actor->entryTarget228;
        u32 one = 1;
        u8 *taken = &actor->field2B2;
        u32 zero = 0;
        *taken = one;
        actor->field2B0 = zero;
        actor->field2B5 = one;
        actor->target = fn;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
        Trigger_ConsumePending((void *)((u8 *)actor + 0xC), 1);
        {
            u32 mask = 0x800;
            u16 *flags;
            flags = &actor->flags2C0;
            mask |= *flags;
            *flags = mask;
        }
    }
    {
        actor->counter2AC++;
    }
}
