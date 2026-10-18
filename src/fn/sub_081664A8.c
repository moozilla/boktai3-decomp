#include "global.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 Div(s32, s32);
void MenuDisplay_LoadAdjustedDigits(s32 value, s32 adjustment, void *data, void *hundredsAnim,
                  void *tensAnim, void *onesAnim)
{
    s32 hundreds, tens, ones, bank;
    value += adjustment;
    hundreds = Div(value, 100);
    value -= hundreds * 100;
    tens = Div(value, 10);
    value -= tens * 10;
    ones = value;
    bank = 10;
    if (adjustment <= 0) {
        bank = 0;
        if (adjustment < 0)
            bank = 20;
    }
    Anim_LoadKeyframe(hundredsAnim, data, (u16)(bank + hundreds), 1);
    Anim_LoadKeyframe(tensAnim, data, (u16)(bank + tens), 1);
    Anim_LoadKeyframe(onesAnim, data, (u16)(bank + ones), 1);
}
