#include "global.h"
#include "scene_actor_reset.h"
struct SceneActorMotionParent {
    u8 unknown0[0x6F2];
    u16 flags;
};
void Mummy_EnterIdleControl(void *);
void SceneActorControl_OnIdle(void);
static inline u8 MotionStarted(struct SceneActorReactionMotion *motion)
{
    u32 mask = 8;
    if (motion->flags & mask)
        return 1;
    return 0;
}
void SceneActor_EnterMotionPhase(struct SceneActorRegistryResetView *actor)
{
    u32 *flags = &actor->flags2D0;
    s32 mask = -5;
    struct SceneActorMotionParent *parent;
    u32 started;
    *flags &= mask;
    parent = actor->context;
    {
        u16 *p = &parent->flags;
        u32 mask = 0xFFFFEBFF;
        *p = mask & *p;
    }
    started = MotionStarted((void *)&actor->tracking.target);
    if (started) {
        u16 *motionFlags = &actor->group.common.gate.flags;
        {
            u32 mask = 4;
            u32 value = *motionFlags;
            mask |= value;
            *motionFlags = mask;
        }
        {
            u32 mask = 0x80;
            motionFlags = &actor->flags2D4;
            {
                u32 value = *motionFlags;
                mask |= value;
                *motionFlags = mask;
            }
        }
        {
            u32 mask;
            actor->group.common.gate.counter = actor->group.common.gate.duration;
            mask = 0x100;
            if (*motionFlags & mask)
                return;
        }
        Mummy_EnterIdleControl(actor);
    } else {
        u32 one = 1;
        actor->state2A8 = 0;
        actor->phase2A9 = one;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = one;
        {
            void *control = SceneActorControl_OnIdle;
            u32 kind = 0x10;
            actor->field2B2 = one;
            actor->field2B0 = 0;
            actor->field2B5 = kind;
            actor->target = control;
            actor->field2C8 = 0;
            {
                u32 *flags = &actor->flags114;
                s32 mask = -2;
                *flags &= mask;
            }
            actor->word104 = 0;
        }
    }
    {
        struct SceneActorReactionMotion *motion = (void *)&actor->tracking.target;
        u32 mask = 8;
        {
            u32 value = motion->flags;
            mask |= value;
            motion->flags = mask;
        }
    }
}
