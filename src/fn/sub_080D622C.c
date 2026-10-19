#include "scene_actor_control_phase.h"
/* SceneActorControl_UpdatePersistentPhase_080D622C: SceneActorControl_UpdatePersistentPhase variant: control sub_080D3D60 kind 39, no shadow toggle, update callback via Callback_InvokeActor (provisional name) */
#define SceneActorControl_UpdatePersistentPhase_080D622C sub_080D622C
#include "scene_actor_reaction.h"
void sub_080D3D60(struct SceneActorRegistryResetView *);
s32 Callback_InvokeActor(void *, void *);
void SceneActorControl_UpdatePersistentPhase_080D622C(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *control = (void *)sub_080D3D60;
        u32 kind;
        kind = 39;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = control;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
    }
    if (!(u8)Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270)))
        ++actor->counter2AC;
}
