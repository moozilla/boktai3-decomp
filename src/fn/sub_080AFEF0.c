#include "scene_actor_chase.h"
// confidence=low: completed action chooses a pursuit callback from context flag one.
void Vorn_UpdatePursuitWait(void);
void Vorn_UpdatePursuitDistance(void);
static inline void SelectPursuit(struct ChaseActor *a, void *cb, u32 mode)
{
    void *callback = cb;
    u32 zero = 0;
    u32 kind = mode;
    a->base.pendingControl = (u32)callback;
    a->base.pendingControlData = zero;
    a->base.pendingControlRelease = zero;
    a->base.pendingControlKind = kind;
    a->base.pendingControlFlags = 1;
}
void Vorn_SelectPursuitAfterAction(struct ChaseActor *a)
{
    u8 *context = a->base.context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, a->base.onEntry, 2);
    Callback_InvokeActor(a, a->base.contactQuery.opaque);
    if (a->base.stepDone) {
        Chase_SetBehavior(a, 15);
        if (Chase_Test(*(u16 *)(context + 0x6D4), 1))
            SelectPursuit(a, Vorn_UpdatePursuitWait, 0x23);
        else
            SelectPursuit(a, Vorn_UpdatePursuitDistance, 0x25);
    } else
        a->base.counter2AC++;
}
