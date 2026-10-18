#define StatGauge_WaitBeforeFade sub_081BD184
#define StatGauge_SetCallback DarkLoan_SetState
#include "global.h"
void DarkLoan_SetState(void *, void (*)(void));
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_081BD1D0(void);
struct StatGaugePreview {
    u8 unknown0[0x904];
    u16 timer;
};
void sub_081BD184(struct StatGaugePreview *p)
{
    if (++p->timer > 149) {
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
        DarkLoan_SetState(p, sub_081BD1D0);
    }
}
