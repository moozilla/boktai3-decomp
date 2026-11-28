#include "global.h"
#include "link/link_script_panel.h"

u8 LinkSession_ConsumeStateEntry(struct LinkScriptPanel *);
void LinkSession_SetState(struct LinkScriptPanel *, u32, void *);
void SerialLink_InitMultiplayer(void);
s32 Serial_DetectRoleAndEnableInterrupt(void);
void Serial_StopCommunication(void);
void ScriptPanel_WaitForSelectionData(void);
s32 ScriptPanel_PollSelection(struct LinkScriptPanel *p);

static inline void ScriptPanel_Retry(struct LinkScriptPanel *p)
{
    p->retryTimer++;
    if (p->retryTimer > 29) {
        u8 *base, *button;
        u32 zero;
        LinkSession_SetState(p, 2, ScriptPanel_PollSelection);
        Serial_StopCommunication();
        base = p->buttons;
        zero = 0;
        button = &p->buttons[1];
        do {
            *button-- = zero;
        } while ((s32)button >= (s32)base);
    }
}

// confidence=low: starts multiplayer serial, then waits for a role and retries on timeout.
s32 ScriptPanel_PollSelection(struct LinkScriptPanel *p)
{
    struct LinkScriptPanel *q = p;
    s32 i;
    s32 role;

    if (LinkSession_ConsumeStateEntry(p)) {
        SerialLink_InitMultiplayer();
        i = 0;
        do {
            p->buttons[i] = 0;
            p->masks[i] |= 0xFFFF;
        } while (++i <= 1);
        p->scale = 0;
        p->retryTimer = 0;
        return 0;
    }
    if (p->counter > 19) {
        role = Serial_DetectRoleAndEnableInterrupt();
        if (role >= 0) {
            p->role = role;
            LinkSession_SetState(p, 3, ScriptPanel_WaitForSelectionData);
        } else {
            ScriptPanel_Retry(q);
        }
    }
    return 0;
}
