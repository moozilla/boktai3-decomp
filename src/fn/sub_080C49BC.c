#include "global.h"
#include "scene_actor_reset.h"
#define SceneActorControl_RefreshMotionRoute sub_080C49BC
void sub_080C4EF0(void *);
void sub_0807FC90(void);
static inline u32 TestFlags(u16 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 MotionActive(struct SceneActorReactionMotion *motion)
{
    u32 mask = 8;
    if (motion->flags & mask)
        return 1;
    return 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 zero)
{
    u32 phase = 1;
    actor->state2A8 = zero;
    actor->phase2A9 = phase;
    actor->behavior2AA = zero;
    actor->counter2AC = zero;
    actor->field2B1 = phase;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind,
                              u32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
void sub_080C49BC(struct SceneActorRegistryResetView *actor)
{
    u32 active = MotionActive((void *)&actor->fieldDC);
    if (active) {
        {
            u16 *flags = &actor->group.flags414;
            u32 mask = 4;
            u32 value = *flags;
            mask |= value;
            *flags = mask;
        }
        {
            u32 mask = 128;
            u16 *flags = &actor->flags2D4;
            u32 value = *flags;
            mask |= value;
            *flags = mask;
        }
        actor->group.field418 = actor->group.field416;
        if (TestFlags(&actor->flags2D4, 256))
            return;
        sub_080C4EF0(actor);
    } else {
        SetState(actor, active);
        SetControl(actor, (void *)sub_0807FC90, 16, active);
    }
    {
        struct SceneActorReactionMotion *motion = (void *)&actor->fieldDC;
        u32 mask = 8;
        u32 value = motion->flags;
        mask |= value;
        motion->flags = mask;
    }
}
