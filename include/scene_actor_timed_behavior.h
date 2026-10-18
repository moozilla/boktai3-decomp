#ifndef SCENE_ACTOR_TIMED_BEHAVIOR_H
#define SCENE_ACTOR_TIMED_BEHAVIOR_H
#include "scene_actor_reset.h"
/* Narrow view of actor->context used by the behavior 56/57 helpers. */
struct SceneActorTimedBehaviorContext {
    u8 unknown0[0x6D0];
    u8 field6D0;
    u8 unknown6D1[0xB];
    u32 flags6DC;
    u32 nearRange6E0;
    u8 unknown6E4[0x1F];
    u8 attempts703;
    u16 timer704;
};
struct SceneActorFacingTarget {
    u8 unknown0[9];
    u8 heading;
};
#endif
