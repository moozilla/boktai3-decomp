#ifndef GUARD_SCENE_ACTOR_CONTROL_PHASE_H
#define GUARD_SCENE_ACTOR_CONTROL_PHASE_H
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_actor_behavior.h"
/* Tentative view of actor3D0's control context, shared by the 082A6C family. */
struct SceneActorControlState {
    u8 unknown0[0x3D4];
    u32 flags3D4;
    u8 unknown3D8[2];
    u8 pulseInterval, unknown3DB;
    u8 maxElapsed, savedDirection;
    u8 unknown3DE[8];
    u8 elapsed, startTick;
    u8 unknown3E8[5];
    u8 randomDelay3ED;
};
struct SceneActorControlPhaseView {
    u8 unknown0[0x3D0];
    struct SceneActorControlState *control;
};
static inline struct SceneActorControlState *
SceneActorControl_GetState(struct SceneActorRegistryResetView *actor)
{
    return ((struct SceneActorControlPhaseView *)actor)->control;
}
static inline u8 SceneActorControl_TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline u8 SceneActorControl_TakeStateEntry(struct SceneActorRegistryResetView *actor)
{
    return SceneActorControl_TakeFlag(&actor->field2B1);
}

#endif
