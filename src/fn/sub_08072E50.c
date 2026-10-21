#include "scene_actor_contact_list.h"
static inline u8 TestGate(struct SceneActorContactGateView *gate, u32 mask)
{
    if (gate->flags & mask)
        return 1;
    return 0;
}
void SceneActorContacts_SetDistanceGate(struct SceneActorRegistryActor *actor,
                                        struct SceneActorContactGateView *gate, u32 metric,
                                        u32 threshold, u32 mode, u32 flags)
{
    flags &= 6;
    if (flags) {
        u32 result;
        if (!threshold && !mode)
            goto yes;
        result = TestGate((void *)((u32)actor + 0xDC), 4);
        if (result)
            goto yes;
        if (mode == 2)
            goto write;
        if (metric > threshold)
            goto write;
    yes:
        result = 1;
    write:
        do {
            gate->enabled = result;
        } while (0);
    } else {
        gate->enabled = flags;
    }
}
