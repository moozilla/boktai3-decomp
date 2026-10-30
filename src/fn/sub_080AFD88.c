#include "scene_actor_chase.h"
// confidence=low: follows a navigation descriptor, switching between acceleration and turning.
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, s32, s32, s32);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, s32, s32, s32);
static inline u32 TestFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D4 & mask;
}
void Vorn_UpdateDescriptorPursuit(struct SceneActorRegistryResetView *a)
{
    u8 *group = (u8 *)&a->group;
    u8 *context = a->context;
    s32 motion;
    s32 event;
    if (a->field2B1) {
        a->field2B1 = 0;
    }
    Callback_InvokeActor(a, a->contactQuery.opaque);
    motion = Callback_InvokeActor(a, *(void **)((u8 *)a + 0x260));
    {
        u16 *rate;
        a->field2C2 = 32;
        {
            struct SceneActorRegistryResetView *held = a;
            a = held;
        }
        if (TestFlags(a, 0xA02F)) {
            u16 *p = &a->field2C2;
            *p = 64;
            rate = p;
        } else {
            u16 *p = &a->field2C2;
            u16 value = *p;
            rate = p;
            if (!value) {
                *rate = 64;
            }
        }
        if (!a->drawableKind)
            Anim_SetDurationScale((u8 *)a->drawable + 0x48, *rate);
        else
            Anim_SetFrameDurationScale((u8 *)a->drawable + 0x20, *rate);
    }
    event = NavMesh_ConsumeProbeEvent(*(void **)(group + 0x1FC));
    if (!event) {
        if (SceneActorBehavior_AccelerateTowardDescriptor(a, *(void **)(group + 0x1FC), 32, 32,
                                                          motion)) {
            {
                u32 state = 3;
                a->state2A8 = state;
                a->phase2A9 = event;
                a->behavior2AA = event;
                a->counter2AC = event;
                a->field2B1 = 1;
            }
            {
                u32 m = 2;
                u16 *flags;
                flags = (void *)(context + 0x6D4);
                *flags = (*(&m)) | (*flags);
            }
            return;
        }
    } else {
        if (SceneActorBehavior_RequestDescriptorTurn(a, *(void **)(group + 0x1FC), 32, 32, motion))
            Chase_SetBehavior(a, 16);
    }
    a->counter2AC++;
}
