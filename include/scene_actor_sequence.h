#ifndef GUARD_SCENE_ACTOR_SEQUENCE_H
#define GUARD_SCENE_ACTOR_SEQUENCE_H

#include "global.h"

struct SceneActorSequence {
    u8 task[0x18];
    u8 *background;
    u8 *palette;
    s16 layer3X;
    s16 layer3Y;
    u8 unknown24[4];
    s16 layer1X;
    s16 layer1Y;
    u8 unknown2C[4];
    void *actor;
    u8 unknown34[8];
    void *completionScript;
    u8 unknown40[0x10];
    void (*updateState)(struct SceneActorSequence *);
    s32 timer;
    s16 secondWaypointSpeed;
    s16 firstWaypointSpeed;
    s16 finalWaypointSpeed;
    u8 unknown5E[2];
};

struct SceneActorSequencePoint {
    s16 x;
    s16 y;
    s16 z;
};

struct SceneActorSequenceActor {
    u8 unknown00[0x1C];
    u32 flags;
    u8 unknown20[0x0C];
    s16 animationFrame;
    u8 unknown2E[0x56];
    s16 animationTimer;
    s16 animationState;
};

#endif
