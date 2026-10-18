#include "global.h"
struct ScriptPanel {
    u8 unk0[0x2E];
    u16 retryTimer;
    u8 unk30[0x18];
    s32 selection;
    u8 unk4C[0x18];
    u8 buttons[2];
};
u8 Registry1745_ConsumeStateEntry(struct ScriptPanel *);
void Registry1745_SetState(struct ScriptPanel *, u32, void *);
s32 Serial_ReadLatestPacket(u16 *);
void sub_08109F1C(void);
void ScriptPanel_PollSelection(void);
void Serial_StopCommunication(void);
s32 ScriptPanel_WaitForSelectionData(struct ScriptPanel *p)
{
    u16 selections[4];
    s32 result;
    if (Registry1745_ConsumeStateEntry(p))
        p->retryTimer = 0;
    result = Serial_ReadLatestPacket(selections);
    if (p->selection >= 0 && result >= 0 && selections[p->selection] != 0xFFFF) {
        Registry1745_SetState(p, 4, sub_08109F1C);
    } else {
        p->retryTimer++;
        if (p->retryTimer > 29) {
            u8 *base, *button;
            u32 zero;
            Registry1745_SetState(p, 2, ScriptPanel_PollSelection);
            Serial_StopCommunication();
            base = p->buttons;
            zero = 0;
            button = &p->buttons[1];
            do {
                *button-- = zero;
            } while ((s32)button >= (s32)base);
        }
    }
    return 0;
}
