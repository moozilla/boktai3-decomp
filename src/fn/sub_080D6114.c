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
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    u8 *prepared = &actor->field2B2;
    u32 zero = 0;
    *prepared = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
void SceneActor_WaitForControlCompletion(struct SceneActorRegistryResetView *actor)
{
    u8 busy;
    if (TakePrepared(&actor->field2B1)) {
        void *control = actor->entryTarget224;
        SetControl(actor, control, 2);
    }
    busy = Callback_InvokeActor(actor, actor->contactQuery.opaque);
    if (!busy) {
        if (actor->field2B0) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            {
                u32 state = 0x10;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = state;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
            SetControl(actor, Mummy_UpdatePlayerRoute, 0x27);
        } else
            actor->counter2AC++;
    }
}
