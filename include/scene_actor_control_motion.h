#ifndef GUARD_SCENE_ACTOR_CONTROL_MOTION_H
#define GUARD_SCENE_ACTOR_CONTROL_MOTION_H
#include "scene_actor_control_phase.h"
#include "display_animation.h"
/* Tentative motion-phase extension of SceneActorControlState. */
struct SceneActorControlMotionState {
    u8 unknown0[0x3D4];
    u32 flags3D4;
    u8 unknown3D8[2];
    u8 pulseInterval, field3DB;
    u8 maxElapsed, savedDirection;
    u16 baseOffset;
    u16 field3E0;
    u8 phase;
    s8 swayStep;
    s16 sway;
    u8 elapsed, startTick;
    u8 unknown3E8[2];
    u16 previousGround;
    u8 unknown3EC[0x30];
    struct ContactPosition vectors[3];
    struct ContactPosition storedPosition;
};
struct SceneActorControlMotionDrawable {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[0x36];
    struct DisplaySequenceState sequence;
};
struct SceneActorControlGroundView {
    u8 unknown0[0x2BE];
    u16 ground;
};
static inline struct SceneActorControlMotionState *
SceneActorControl_GetMotionState(struct SceneActorRegistryResetView *actor)
{
    return (void *)SceneActorControl_GetState(actor);
}
#endif
