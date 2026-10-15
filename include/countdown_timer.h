#ifndef GUARD_COUNTDOWN_TIMER_H
#define GUARD_COUNTDOWN_TIMER_H
#include "engine.h"
struct CountdownTimer {
    u8 task[0x18];
    u16 remaining, locked;
    u32 completionScriptId;
};
extern struct CountdownTimer *gUnk_0200049C;
void sub_081783C0(u16 *);
void sub_08178404(void);
void CountdownTimer_Stop(void);
s32 CountdownTimer_Update(struct CountdownTimer *);
void CountdownTimer_Destroy(void);
s32 CountdownTimer_InitFromScript(struct CountdownTimer *, void *, void *);
#endif
