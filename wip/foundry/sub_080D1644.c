#define SceneActor_BeginRecoveryPhase sub_080D1644
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
void sub_0807C72C(struct SceneActorRegistryResetView *, u32, u32, u32);
void sub_080DA598(struct SceneActorRegistryResetView *);
void sub_080D1784(void);
s32 Div(s32, s32);
static inline u8 TakeFlag(u8 *flag) { if (*flag) { *flag = 0; return 1; } return 0; }
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor)
{
    void *callback = sub_080D1784;
    u8 kind = 0x13;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    {
        u32 off = 0x2c8;
        u16 *field = (void *)((u8 *)actor + off);
        *field = 0;
    }
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
void sub_080D1644(struct SceneActorRegistryResetView *actor)
{
    u8 *control = (u8 *)actor + 0x3d4;
    u16 *interval;
    u8 zero;
    u16 count;
    if (TakeFlag(&actor->field2B2)) {
        u32 mask;
        sub_0807C72C(actor, 0, 127, 0);
        sub_0807C72C(actor, 1, 127, 0);
        sub_0807C72C(actor, 3, 127, 0);
        mask = 0x10;
        mask |= actor->flags2D4;
        actor->flags2D4 = mask;
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    {
        s32 value = Div(64, 6);
        interval = &actor->field2C2;
        zero = 0;
        *interval = value;
    }
    count = *(u16 *)(control + 0x1f0);
    if (!count) {
        u32 one;
        u8 *ready = &actor->field2B0;
        u32 duration;
        u32 state;
        one = 1;
        *ready = one;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 6);
        sub_080DA598(actor);
        duration = 64;
        actor->field1EA = duration;
        SetAnimation(actor, 3, zero, zero);
        SetControl(actor);
        {
            u16 *flags = &actor->flags2D4;
            u32 mask = ~0x30;
            mask &= *flags;
            *flags = mask;
        }
        {
            u32 mask = 0x10;
            actor->flags2CC |= mask;
        }
        *interval = duration;
    }
}
