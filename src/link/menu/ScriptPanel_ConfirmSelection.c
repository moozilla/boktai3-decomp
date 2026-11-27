#include "global.h"
#include "link/link_script_panel.h"

#define ScriptPanel_SetSelection LinkSession_SetMenuRequest
u8 LinkSession_ConsumeStateEntry(struct LinkScriptPanel *);
void LinkSession_SetState(struct LinkScriptPanel *, u32, void (*)(void));
void LinkSession_SetMenuRequest(u32);
void Script_Call(u32, void *);
void ScriptPanel_PollSelection(void);
s32 ScriptPanel_ConfirmSelection(struct LinkScriptPanel *p)
{
    if (LinkSession_ConsumeStateEntry(p)) {
        s32 i;
        p->selection = 0;
        for (i = 0; i < 4; i++)
            p->buttons[i] = 0;
    }
    if (p->counter > 10)
        LinkSession_SetMenuRequest(3);
    if (p->selection == 3) {
        Script_Call(p->script, 0);
        LinkSession_SetState(p, 2, ScriptPanel_PollSelection);
        p->selection = 0;
        return 0;
    }
    return 0;
}
