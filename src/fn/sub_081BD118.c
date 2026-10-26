#define StatGauge_SetCallback DarkLoan_SetState
#include "global.h"
#include "game_state.h"
void DarkLoan_SetState(void *, void (*)(void));
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void StatGauge_ShowCompletionMessage(void *);
void StatGauge_CloseAfterDelay(void);
struct StatGaugePreview {
    u8 unknown0[0x904];
    u16 timer;
};
void StatGauge_WaitAfterConfirm(struct StatGaugePreview *p)
{
    if (++p->timer > 149) {
        if (*(s16 *)&gGameState->unk524[0x10] == 0) {
            ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
            DarkLoan_SetState(p, StatGauge_CloseAfterDelay);
        } else {
            StatGauge_ShowCompletionMessage(p);
        }
    }
}
