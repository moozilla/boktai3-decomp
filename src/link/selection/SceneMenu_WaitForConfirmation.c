#include "global.h"
#include "link/selection/link_selection_mode.h"
#include "link/selection/link_selection_prompt.h"
#define SceneMenu_SetState LinkSession_SetState
u8 LinkSession_ConsumeStateEntry(struct LinkSelectionPrompt *);
void LinkSession_BeginShutdown(struct LinkSelectionPrompt *, void (*)(void));
void LinkSession_SetState(struct LinkSelectionPrompt *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void LinkSession_WaitForCompletionFlag(void);
static inline u16 ReadInput(struct LinkSelectionPrompt *p, s32 i)
{
    if (i < 0)
        return 0xFFFF;
    return p->input[i];
}
s32 SceneMenu_WaitForConfirmation(struct LinkSelectionPrompt *p)
{
    s32 completed, i;
    LinkSession_ConsumeStateEntry(p);
    completed = 0;
    for (i = 0; i < p->count; i++) {
        u16 v = ReadInput(p, i);
        if (LinkSelection_IsReady(v, 0x3c00) && (v & 0x3F) == 2)
            completed++;
    }
    if (completed == p->count)
        LinkSession_SetState(p, 0x1B, LinkSession_WaitForCompletionFlag);
    else if (p->state + 2 <= 1 || p->timer > 179) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    p->flags = 0x1002;
}
