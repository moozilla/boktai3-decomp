#define StatGauge_ConfirmPreview sub_081BCB58
#define StatGauge_SetCallback DarkLoan_SetState
#include "global.h"
struct DirectionalScriptArgs;
void Script_Call(u32, struct DirectionalScriptArgs *);
s32 MessageWindow_ShowMessage(s32);
int MessageWindow_SetNumberArgument(int, int);
void DarkLoan_SetState(void *, void (*)(void));
void sub_081BD118(void);
void sub_081BD1D0(void);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
struct StatGaugePreview {
    u8 unknown0[0x8ee];
    u16 bonus;
    u32 confirmScript;
};
void sub_081BCB58(struct StatGaugePreview *p)
{
    if (p->bonus != 0) {
        MessageWindow_ShowMessage(5);
        if (p->confirmScript != 0)
            Script_Call(p->confirmScript, 0);
        else
            MessageWindow_SetNumberArgument(0, 3);
        DarkLoan_SetState(p, sub_081BD118);
    } else {
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
        DarkLoan_SetState(p, sub_081BD1D0);
    }
}
