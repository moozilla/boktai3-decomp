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
void sub_0806F7B0(struct SceneRangeWatch *);
u32 sub_0806F838(struct SceneRangeWatch *);
u32 sub_0806F86C(void);
s32 sub_0806F870(struct SceneRangeWatch *);
#endif
