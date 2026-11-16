#include "global.h"
#include "scene_actor_reset.h"
void SceneActorAnimation_SelectDirectionalEntry(void *, u32);
void Dog_LeapTowardTarget(void);
static inline u8 TakePending(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *control, u8 kind)
{
    u32 zero;
    actor->base.field2B2 = 1;
    zero = 0;
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
void Dog_UpdateLeapWindup(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->base.context;
    if (TakePending(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 0xF);
        *(struct ContactPosition *)(context + 0x6D8) =
            *(struct ContactPosition *)(*(u8 **)((u8 *)actor + 0xDC) + 0xC);
    }
    if (actor->base.field2B3)
        BeginControl(actor, Dog_LeapTowardTarget, 0x23);
}
