#ifndef GUARD_COUNTDOWN_TIMER_H
#define GUARD_COUNTDOWN_TIMER_H
#include "engine.h"
struct CountdownTimer {
    struct Task task;
    u16 remaining, locked;
    u32 completionScriptId;
};
/* 0x0200049C */
extern struct CountdownTimer *gCountdownTimer;
void Hud_RegisterTimer(u16 *);
void Hud_ClearTimer(void);
void CountdownTimer_Stop(void);
s32 CountdownTimer_Update(struct CountdownTimer *);
void CountdownTimer_Destroy(void);
s32 CountdownTimer_InitFromScript(struct CountdownTimer *, void *, void *);
u16 CountdownTimer_GetRemaining(void);
void CountdownTimer_StopFromScript(void);
struct CountdownTimer *CountdownTimer_CreateFromScript(void *, void *);
void CountdownTimer_AddFromScript(void);
#endif
