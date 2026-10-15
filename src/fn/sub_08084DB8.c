// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
#define SceneActorControl_ConsumePendingEntry sub_08084DB8
static inline u32 TakeEntry(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
void SceneActorControl_ConsumePendingEntry(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u32 entered = TakeEntry(actor);
    asm("" : "+r"(entered));
    if (entered) {
        void *target = *(void **)((u8 *)actor + 0x228);
        u32 one = 1;
        u32 offset = 0x2B2;
        register u8 *field asm("r0") = (u8 *)actor + offset;
        u32 zero;
        zero = 0;
        *field = one;
        {
            register u32 offset asm("r5") = 0x2B0;
            field = (u8 *)actor + offset;
            *field = zero;
            offset += 5;
            field = (u8 *)actor + offset;
            *field = one;
            field = (u8 *)actor + 0x21C;
            *(void **)field = target;
            offset += 0x13;
            field = (u8 *)actor + offset;
            *(u16 *)field = zero;
        }
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
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
        register u32 offset asm("r5") = 0x2AC;
        register u32 *counter asm("r1");
        asm("" : "+r"(offset));
        counter = (void *)((u8 *)actor + offset);
        (*counter)++;
    }
}
