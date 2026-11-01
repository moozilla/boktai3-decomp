#ifndef GUARD_SCENE_ACTOR_STIMULUS_H
#define GUARD_SCENE_ACTOR_STIMULUS_H

#include "scene_actor_reset.h"

static inline u8 SceneActorStimulus_TestMotion(struct SceneActorReactionMotion *motion, u32 input)
{
    u32 mask = input;
    if (motion->flags & mask)
        return 1;
    return 0;
}

static inline u8 SceneActorStimulus_HasLock(u8 *raw)
{
    if (((struct SceneActorRegistryResetView *)raw)->field2B8 || ((struct SceneActorRegistryResetView *)raw)->colliderA.responseValue)
        return 1;
    return 0;
}

static inline u8 SceneActorStimulus_CheckBusy(struct SceneActorRegistryResetView *actor)
{
    u8 *raw = (u8 *)actor;
    u32 flags2D0, flags2CC;
    u32 *p = &actor->flags2D0;
    {
        u32 mask = ~0x1000000;
        flags2D0 = *p & mask;
        *p = flags2D0;
    }
    {
        u32 mask = 0x200000;
        flags2CC = actor->flags2CC;
        if (!(mask & flags2CC))
            return 1;
    }
    if (flags2D0 & 4)
        return 1;
    if (flags2CC & 0x100)
        return 1;
    {
        u32 mask = 0xE13F;
        if (actor->flags2D4 & mask)
            return 1;
    }
    if (SceneActorStimulus_TestMotion((struct SceneActorReactionMotion *)&actor->tracking.target, 0x10))
        return 1;
    return SceneActorStimulus_HasLock(raw);
}

static inline u16 SceneActorStimulus_CaptureMotionFlags(struct SceneActorRegistryResetView *actor,
                                                        u32 mask, u16 **field)
{
    u16 *flags = &actor->flags2D4;
    u16 value = *flags;
    value &= mask;
    *field = flags;
    return value;
}

static inline void SceneActorStimulus_ExtendGate(struct SceneActorRegistryResetView *actor)
{
    u16 *counter = &actor->group.common.gate.counter;
    *counter += 300;
    if (*counter > actor->group.common.gate.duration)
        *counter = actor->group.common.gate.duration;
}

#endif
