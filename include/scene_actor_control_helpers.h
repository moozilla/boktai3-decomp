#ifndef GUARD_SCENE_ACTOR_CONTROL_HELPERS_H
#define GUARD_SCENE_ACTOR_CONTROL_HELPERS_H
#include "scene_actor_control_motion.h"
/* Phase-dependent helper contracts for actor3D0's control context. */
struct SceneActorControlPositionView {
    u8 unknown0[0x3FC];
    struct ContactPosition position;
};
void sub_08083BFC(struct SceneActorRegistryResetView *, const struct ContactPosition *);
s32 sub_08083C38(struct SceneActorRegistryResetView *);
void SceneActorControl_StartPositionCurve(struct SceneActorRegistryResetView *,
                                          const struct ContactPosition *);
static inline u8 SceneActorControl_TestFlags(u32 flags, u32 mask)
{
    if (flags & mask)
        return 1;
    return 0;
}
#endif
