// MATCHING_CONCESSION: register-pinning
#include "scene_actor_contact_list.h"
#define SceneActorContacts_SetDistanceGate sub_08072E50
static inline u8 TestGate(struct SceneActorContactGateView *gate, u32 mask)
{
    if (gate->flags & mask)
        return 1;
    return 0;
}
void SceneActorContacts_SetDistanceGate(struct SceneActorRegistryActor *inputActor,
                                        struct SceneActorContactGateView *inputGate,
                                        u32 inputMetric, u32 threshold, u32 inputMode,
                                        u32 inputFlags)
{
    struct SceneActorRegistryActor *actor = inputActor;
    register struct SceneActorContactGateView *gate asm("r4") = inputGate;
    u32 metric = inputMetric;
    register u32 mode asm("r6") = inputMode;
    u32 flags = inputFlags;
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
        gate->enabled = result;
    } else {
        gate->enabled = flags;
    }
}
