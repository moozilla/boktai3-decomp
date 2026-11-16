#include "scene_actor_control_phase.h"
/* SceneActorControl_UpdatePersistentPhase_080D622C: SceneActorControl_UpdatePersistentPhase variant: control Mummy_UpdatePlayerRoute kind 39, no shadow toggle, update callback via Callback_InvokeActor (provisional name) */
#include "scene_actor_reaction.h"
void Mummy_UpdatePlayerRoute(struct ChaseActor *);
s32 Callback_InvokeActor(void *, void *);
void SceneActorControl_UpdatePersistentPhase_080D622C(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *control = (void *)Mummy_UpdatePlayerRoute;
        u32 kind;
        kind = 39;
        actor->base.field2B2 = 1;
        actor->base.field2B0 = 0;
        actor->base.field2B5 = kind;
        actor->base.target = control;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.word104 = 0;
    }
    if (!(u8)Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270)))
        ++actor->base.counter2AC;
}
