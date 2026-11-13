#ifndef GUARD_COUNTDOWN_CONTROLLER_H
#define GUARD_COUNTDOWN_CONTROLLER_H

#include "engine.h"
#include "player.h"

struct CountdownTask {
    struct Task task;
    u16 frame, interval;
};

void Hud_UpdateCountdownDisplay(s32 remaining, s32 total, u32 update);
void Player_EnterKindFour(struct Player *player);
s32 Countdown_Tick(struct CountdownTask *task);
u32 Countdown_Destroy(void);
s32 Countdown_InitializeFromScript(struct CountdownTask *task, u32 script, u32 context);
struct CountdownTask *Countdown_CreateFromScript(u32 script, u32 context);

#endif
