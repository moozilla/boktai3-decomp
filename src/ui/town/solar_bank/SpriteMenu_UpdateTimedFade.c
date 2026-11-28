#include "global.h"
#include "ui/town/solar_bank_panel.h"
s32 Anim_Step(void *, void *);
void SolarBank_SetState(void *, void (*)(void));
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void SolarBank_UpdateExitDelay(void);
void SpriteMenu_UpdateTimedFade(struct SolarBankPanel *panel)
{
    Anim_Step(&panel->label, &panel->labelData);
    if (++panel->timer > 149) {
        ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
        SolarBank_SetState(panel, SolarBank_UpdateExitDelay);
    }
}
