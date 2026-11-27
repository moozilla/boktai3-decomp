#include "global.h"

#define ScriptPanel_SetSelection LinkSession_SetMenuRequest
struct ScriptPanel {
    u8 unk0[0x23];
    u8 selection;
    u8 unk24[0xC];
    u32 counter;
    u8 unk34[0x30];
    u8 buttons[4];
    u8 unk68[0x68];
    u32 script;
};
u8 LinkSession_ConsumeStateEntry(struct ScriptPanel *);
void LinkSession_SetState(struct ScriptPanel *, u32, void (*)(void));
void LinkSession_SetMenuRequest(u32);
void Script_Call(u32, void *);
void ScriptPanel_PollSelection(void);
s32 ScriptPanel_ConfirmSelection(struct ScriptPanel *p)
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
