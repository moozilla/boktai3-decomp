#include "global.h"
#include "mummy.h"
#include "scene_actor_reset.h"
void Mummy_EnterIdleControl(void *);
void SceneActorControl_OnIdle(void);
static inline u8 MotionStarted(struct SceneActorReactionMotion *motion)
{
    u32 mask = 8;
    if (motion->flags & mask)
        return 1;
    return 0;
}
void SceneActor_EnterMotionPhase(struct ChaseActor *actor)
{
    u32 *flags = &actor->base.statusFlags;
    s32 mask = -5;
    struct Mummy *parent;
    u32 started;
    *flags &= mask;
    parent = actor->base.context;
    {
        u16 *p = &parent->flags6F2;
        u32 mask = 0xFFFFEBFF;
        *p = mask & *p;
    }
    started = MotionStarted((void *)&actor->base.tracking.target);
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
            motionFlags = &actor->base.motionFlags;
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
        actor->base.state2A8 = 0;
        actor->base.phase2A9 = one;
        actor->base.behavior2AA = 0;
        actor->base.counter2AC = 0;
        actor->base.field2B1 = one;
        {
            void *control = SceneActorControl_OnIdle;
            u32 kind = 0x10;
            actor->base.field2B2 = one;
            actor->base.stepDone = 0;
            actor->base.controlKind = kind;
            actor->base.target = control;
            actor->base.field2C8 = 0;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                s32 mask = -2;
                *flags &= mask;
            }
            actor->base.pendingControl = 0;
        }
    }
    {
        struct SceneActorReactionMotion *motion = (void *)&actor->base.tracking.target;
        u32 mask = 8;
        {
            u32 value = motion->flags;
            mask |= value;
            motion->flags = mask;
        }
    }
}
