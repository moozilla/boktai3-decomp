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
static inline void SetControlWords(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
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
        actor->word104 = zero;
        actor->word104 = (u32)actor->entryTarget228;
        actor->word108 = 0;
        actor->word10C = 0;
        actor->word110 = 1;
        *flags = 1;
    }
}
void SceneActorControl_WaitForRouteResult(struct SceneActorRegistryResetView *actor)
{
    u8 busy;
    u8 zero;
    if (TakePrepared(&actor->field2B1)) {
        void *control = actor->entryTarget224;
        SetControl(actor, control, 2);
    }
    busy = Callback_InvokeActor(actor, actor->update270);
    if (!busy) {
        zero = sub_0812F3CC(actor);
        if (!zero) {
            if (actor->field2B0) {
                u32 state = 0x13;
                actor->state2A8 = zero;
                actor->phase2A9 = zero;
                actor->behavior2AA = state;
                actor->counter2AC = zero;
                actor->field2B1 = 1;
                SetControlWords(actor, actor->savedTarget, 0);
            } else
                actor->counter2AC++;
        }
    }
}
