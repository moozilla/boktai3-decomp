#include "global.h"
#define SolarBank_RefreshAmountDigits sub_081BB3D4

#include "solar_bank_panel.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 Div(s32, s32);
void sub_081BB3D4(struct SolarBankPanel *panel)
{
    s32 digits[4];
    s32 value, remaining;
    s32 *digit;
    struct SolarBankDigit *anim;
    void *data;
    value = panel->transferAmount;
    if (value == 0) {
        data = panel->animData;
        {
            s32 zero = 0;
            s32 *clear = &digits[3];
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)digits);
        }
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
        data = panel->animData;
    }
    digit = digits;
    anim = panel->transferDigits;
    remaining = 3;
    do {
        Anim_LoadKeyframe(anim, data, (u16)*digit++, 1);
        anim++;
        remaining--;
    } while (remaining >= 0);
    value = panel->balance;
    if (value == 0) {
        {
            s32 zero = 0;
            s32 *clear = &digits[3];
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)digits);
        }
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
    }
    digit = digits;
    anim = panel->balanceDigits;
    remaining = 3;
    do {
        Anim_LoadKeyframe(anim, data, (u16)*digit++, 1);
        anim++;
        remaining--;
    } while (remaining >= 0);
}
