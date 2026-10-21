#include "global.h"

#define ScriptPanel_SetSelection Registry1745_SetMenuRequest
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
u8 Registry1745_ConsumeStateEntry(struct ScriptPanel *);
void Registry1745_SetState(struct ScriptPanel *, u32, void (*)(void));
void Registry1745_SetMenuRequest(u32);
void Script_Call(u32, void *);
void ScriptPanel_PollSelection(void);
s32 ScriptPanel_ConfirmSelection(struct ScriptPanel *p)
{
    if (Registry1745_ConsumeStateEntry(p)) {
        u8 *button;
        u8 *base;
        u32 zero;
        p->selection = 0;
        base = p->buttons;
        zero = 0;
        button = &p->buttons[3];
        do {
            *button-- = zero;
        } while ((s32)button >= (s32)base);
    }
    if (p->counter > 10)
        Registry1745_SetMenuRequest(3);
    if (p->selection == 3) {
        Script_Call(p->script, 0);
        Registry1745_SetState(p, 2, ScriptPanel_PollSelection);
        p->selection = 0;
        return 0;
    }
    return 0;
}
