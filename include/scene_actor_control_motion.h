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
    u8 randomMotionAngle, randomDelay3ED;
    u16 visibilityFrame;
    u8 unknown3F0[0xC];
    struct ContactPosition curvePosition;
    u8 unknown404[0x18];
    struct ContactPosition vectors[3];
    struct ContactPosition storedPosition;
};
struct SceneActorControlPresetView {
    u8 unknown0[0x284];
    const void *preset284;
};
struct SceneActorControlGroundView {
    u8 unknown0[0x2BE];
    u16 ground;
};
static inline struct SceneActorControlMotionState *
SceneActorControl_GetMotionState(struct ChaseActor *actor)
{
    return (void *)SceneActorControl_GetState(actor);
}
void SceneActorControl_SetDefaultPreset(struct ChaseActor *);
/* Phase-dispatch callers supply an actor with an initialized control context. */
void SceneActorControl_ConsumeLiftFlag(struct ChaseActor *);
void SceneActorControl_TickGroundBob(struct ChaseActor *);
void SceneActorControl_TickSway(struct ChaseActor *);
#endif
