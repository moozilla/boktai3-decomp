#include "scene_actor_chase.h"
// confidence=low: completed action chooses a pursuit callback from context flag one.
void sub_080ADB90(void);
void Vorn_UpdatePursuitDistance(void);
static inline void SelectPursuit(struct SceneActorRegistryResetView *a, void *cb, u32 mode)
{
    void *callback = cb;
    u32 zero = 0;
    u32 kind = mode;
    a->word104 = (u32)callback;
    a->word108 = zero;
    a->word10C = zero;
    a->word110 = kind;
    a->flags114 = 1;
}
void Vorn_SelectPursuitAfterAction(struct SceneActorRegistryResetView *a)
{
    u8 *context = a->context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, a->entryTarget224, 2);
    Callback_InvokeActor(a, a->contactQuery.opaque);
    if (a->field2B0) {
        Chase_SetBehavior(a, 15);
        if (Chase_Test(*(u16 *)(context + 0x6D4), 1))
            SelectPursuit(a, sub_080ADB90, 0x23);
        else
            SelectPursuit(a, Vorn_UpdatePursuitDistance, 0x25);
    } else
        a->counter2AC++;
}
