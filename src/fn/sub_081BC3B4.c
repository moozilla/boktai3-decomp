#include "global.h"
#include "game_state.h"

struct DarkLoanDigitAnim {
    u8 data[0x60];
};

struct DarkLoanDisplay {
    u8 unknown0[0x38];
    u8 digitResource[0x298 - 0x38];
    struct DarkLoanDigitAnim digits[4];
    u8 unknown418[0x8EC - 0x418];
    u16 amountPerStep;
    u16 selectedSteps;
};

s32 Div(s32, s32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);

void DarkLoan_UpdateBalanceDigits(struct DarkLoanDisplay *s)
{
    s32 digits[4];
    s32 value;
    s32 *p;
    u8 *resource;
    struct DarkLoanDigitAnim *anim;
    s32 i;

    value = gGameState->menuRewardBalance + s->selectedSteps * s->amountPerStep;
    if (value == 0) {
        resource = s->digitResource;
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
        resource = s->digitResource;
    }
    p = digits;
    anim = s->digits;
    i = 3;
    do {
        Anim_LoadKeyframe(anim, resource, (u16)(*p++ + 20), 1);
        anim++;
    } while (--i >= 0);
}
