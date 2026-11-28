#include "global.h"

#include "ui/town/solar_bank_panel.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 Div(s32, s32);
void SolarBank_RefreshAmountDigits(struct SolarBankPanel *panel)
{
    s32 digits[4];
    s32 value, i;
    value = panel->transferAmount;
    if (value == 0) {
        for (i = 0; i < 4; i++)
            digits[i] = 0;
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
    }
    for (i = 0; i < 4; i++)
        Anim_LoadKeyframe(&panel->transferDigits[i], &panel->animData, (u16)digits[i], 1);
    value = panel->balance;
    if (value == 0) {
        for (i = 0; i < 4; i++)
            digits[i] = 0;
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
    }
    for (i = 0; i < 4; i++)
        Anim_LoadKeyframe(&panel->balanceDigits[i], &panel->animData, (u16)digits[i], 1);
}
