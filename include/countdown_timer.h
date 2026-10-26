#ifndef GUARD_COUNTDOWN_TIMER_H
#define GUARD_COUNTDOWN_TIMER_H
#include "engine.h"
struct CountdownTimer {
    u8 task[0x18];
    u16 remaining, locked;
    u32 completionScriptId;
};
extern struct CountdownTimer *gCountdownTimer;
void Hud_RegisterTimer(u16 *);
void Hud_ClearTimer(void);
void CountdownTimer_Stop(void);
s32 CountdownTimer_Update(struct CountdownTimer *);
void CountdownTimer_Destroy(void);
s32 CountdownTimer_InitFromScript(struct CountdownTimer *, void *, void *);
#endif
