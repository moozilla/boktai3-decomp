#ifndef GUARD_SCENE_RANGE_WATCH_H
#define GUARD_SCENE_RANGE_WATCH_H
#include "engine.h"
#include "game_state.h"
/* Script task watching the inclusive range of game-state field876. */
struct SceneRangeWatch {
    u8 task[0x18];
    u16 enterScript, leaveScript;
    u32 arguments[2];
    u16 interval, frame, inside;
    u8 minimum, maximum;
};
void SceneRangeWatch_CheckRange(struct SceneRangeWatch *);
u32 SceneRangeWatch_Tick(struct SceneRangeWatch *);
u32 SceneRangeWatch_Destroy(void *);
s32 SceneRangeWatch_Initialize(struct SceneRangeWatch *);
#endif
