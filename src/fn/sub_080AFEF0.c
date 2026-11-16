#include "scene_actor_chase.h"
// confidence=low: completed action chooses a pursuit callback from context flag one.
void sub_080ADB90(void);
void Vorn_UpdatePursuitDistance(void);
static inline void SelectPursuit(struct ChaseActor *a, void *cb, u32 mode)
{
    void *callback = cb;
    u32 zero = 0;
    u32 kind = mode;
    a->base.word104 = (u32)callback;
    a->base.word108 = zero;
    a->base.word10C = zero;
    a->base.word110 = kind;
    a->base.flags114 = 1;
}
void Vorn_SelectPursuitAfterAction(struct ChaseActor *a)
{
    u8 *context = a->base.context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, a->base.entryTarget224, 2);
    Callback_InvokeActor(a, a->base.contactQuery.opaque);
    if (a->base.field2B0) {
        Chase_SetBehavior(a, 15);
        if (Chase_Test(*(u16 *)(context + 0x6D4), 1))
            SelectPursuit(a, sub_080ADB90, 0x23);
        else
            SelectPursuit(a, Vorn_UpdatePursuitDistance, 0x25);
    } else
        a->base.counter2AC++;
}
