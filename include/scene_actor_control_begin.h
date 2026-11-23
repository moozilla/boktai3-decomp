#ifndef GUARD_SCENE_ACTOR_CONTROL_BEGIN_H
#define GUARD_SCENE_ACTOR_CONTROL_BEGIN_H

#include "chase_actor.h"

/* Installs a control with the zero the caller already holds in a register: the route and
   contact phases pass a value known to be zero rather than a literal. */
static inline void SceneActorControl_Begin(struct ChaseActor *actor, void *control, u32 kind,
                                           u32 zero)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}

#endif
