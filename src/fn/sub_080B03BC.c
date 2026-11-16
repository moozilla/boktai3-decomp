#include "global.h"
#include "scene_actor_reset.h"
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(void *);
static inline u8 TakePrepared(u8 *field)
{
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct ChaseActor *actor, void *control, u32 kind)
{
    u8 *prepared = &actor->base.field2B2;
    u32 zero = 0;
    *prepared = 1;
    actor->base.field2B0 = zero;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.word104 = zero;
}
static inline void SetControlWords(struct ChaseActor *actor, void *control, u32 kind)
{
    u8 *prepared = &actor->base.field2B2;
    u32 zero = 0;
    *prepared = 1;
    actor->base.field2B0 = zero;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = -2;
        *flags &= mask;
        actor->base.word104 = zero;
        actor->base.word104 =
            *(u32 *)&actor->base.onAlternateEntry; /* a u32 read keeps the store above */
        actor->base.word108 = 0;
        actor->base.word10C = 0;
        actor->base.word110 = 1;
        *flags = 1;
    }
}
// confidence=low: Ghoul_WaitForRouteResult's twin; on a route result, resets to behavior 0x13
// and returns control to savedTarget with word104 = entryTarget228.
void SceneActorControl_WaitForRouteResult(struct ChaseActor *actor)
{
    u8 busy;
    u8 zero;
    if (TakePrepared(&actor->base.field2B1)) {
        void *control = actor->base.onEntry;
        SetControl(actor, control, 2);
    }
    busy = Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    if (!busy) {
        zero = sub_0812F3CC(actor);
        if (!zero) {
            if (actor->base.field2B0) {
                u32 state = 0x13;
                actor->base.state2A8 = zero;
                actor->base.phase2A9 = zero;
                actor->base.behavior2AA = state;
                actor->base.counter2AC = zero;
                actor->base.field2B1 = 1;
                SetControlWords(actor, actor->base.onStanding, 0);
            } else
                actor->base.counter2AC++;
        }
    }
}
