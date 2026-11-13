#ifndef GUARD_MESSAGE_PANEL_H
#define GUARD_MESSAGE_PANEL_H
#include "engine.h"
/* Link outcome and save notices share this task and its state dispatcher. */
struct MessagePanel {
    struct Task task;
    void *backgroundResource;
    const u16 *backgroundPalette;
    void (*state)(struct MessagePanel *);
    const u8 *script, *closeScript;
    u32 flags;
    s32 window;
    u8 mode, entering;
    u16 timer;
    u32 result, connectionKey, unknown40, exitScript, outcomeScript;
};
void MessagePanel_LoadBackground(struct MessagePanel *);
void MessagePanel_CreateTextWindow(struct MessagePanel *);
void MessagePanel_HideTextWindow(struct MessagePanel *);
void MessagePanel_RunExitScript(struct MessagePanel *);
void MessagePanel_ShowOutcome(struct MessagePanel *, u32, s32, u32);
void MessagePanel_RunOutcomeScript(struct MessagePanel *, u32, u32, u32);
s32 MessagePanel_GetErrorMessageIndex(struct MessagePanel *);
void MessagePanel_ShowNotice(struct MessagePanel *);
void MessagePanel_ShowFollowup(struct MessagePanel *);
void MessagePanel_SelectNextState(struct MessagePanel *);
void MessagePanel_WaitForDismiss(struct MessagePanel *);
void MessagePanel_WaitForInput(struct MessagePanel *);
void MessagePanel_OnExitState(struct MessagePanel *);
void MessagePanel_BeginDismiss(struct MessagePanel *);
void MessagePanel_DelayDismiss(struct MessagePanel *);
void MessagePanel_ResetDisplay(struct MessagePanel *);
s32 MessagePanel_Update(struct MessagePanel *);
s32 MessagePanel_OnDestroy(struct MessagePanel *);
s32 MessagePanel_InitFromScript(struct MessagePanel *);
struct MessagePanel *MessagePanel_CreateFromScript(void);
#endif
