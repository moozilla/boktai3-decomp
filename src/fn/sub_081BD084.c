#define StatGauge_WaitBeforeConfirmation sub_081BD084
#include "global.h"
struct StatGaugePreview {
    u8 unknown0[0x18];
    u8 backgroundData[0x40];
    u8 backgroundAnim[0x60];
    u8 unknownB8[0x836];
    u16 bonus;
    u32 confirmScript;
    u8 unknown8F4[0x10];
    u16 timer;
};
struct DirectionalScriptArgs;
s32 Anim_Step(void *, void *);
void Script_Call(u32, struct DirectionalScriptArgs *);
s32 MessageWindow_ShowMessage(s32);
int MessageWindow_SetNumberArgument(int, int);
void DarkLoan_SetState(void *, void (*)(void));
void StatGauge_WaitAfterConfirm(void);
void StatGauge_CloseAfterDelay(void);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_081BD084(struct StatGaugePreview *p)
{
    Anim_Step(p->backgroundAnim, p->backgroundData);
    if (++p->timer > 149) {
        if (p->bonus != 0) {
            MessageWindow_ShowMessage(5);
            if (p->confirmScript != 0)
                Script_Call(p->confirmScript, 0);
            else
                MessageWindow_SetNumberArgument(0, 3);
            DarkLoan_SetState(p, StatGauge_WaitAfterConfirm);
        } else {
            ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
            DarkLoan_SetState(p, StatGauge_CloseAfterDelay);
        }
    }
}
