#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
s32 Callback_InvokeActor(void *, void *);
void Mummy_UpdatePlayerRoute(void);
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
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}
void SceneActor_WaitForControlCompletion(struct ChaseActor *actor)
{
    u8 busy;
    if (TakePrepared(&actor->base.field2B1)) {
        void *control = actor->base.onEntry;
        SetControl(actor, control, 2);
    }
    busy = Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    if (!busy) {
        if (actor->base.stepDone) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            {
                u32 state = 0x10;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = state;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            SetControl(actor, Mummy_UpdatePlayerRoute, 0x27);
        } else
            actor->base.counter2AC++;
    }
}
