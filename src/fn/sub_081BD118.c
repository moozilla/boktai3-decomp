#define StatGauge_WaitAfterConfirm sub_081BD118
#define StatGauge_SetCallback DarkLoan_SetState
#define StatGauge_ShowCompletionMessage sub_081BC9EC
#define gGameState gUnk_02000710
#include "global.h"
#include "game_state.h"
void DarkLoan_SetState(void *, void (*)(void));
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_081BC9EC(void *);
void sub_081BD1D0(void);
struct StatGaugePreview {
    u8 unknown0[0x904];
    u16 timer;
};
void sub_081BD118(struct StatGaugePreview *p)
{
    if (++p->timer > 149) {
        if (*(s16 *)&gGameState->unk524[0x10] == 0) {
            ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
            DarkLoan_SetState(p, sub_081BD1D0);
        } else {
            sub_081BC9EC(p);
        }
    }
}
