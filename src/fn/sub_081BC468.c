#include "global.h"
struct DarkLoanWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x60 - 0xC];
};
struct DarkLoanAdjustment {
    u8 task[0x18];
    u8 resource[0x418 - 0x18];
    struct DarkLoanWidget digits[4], separator, amountDigits[4];
    u8 unknown778[0x8EC - 0x778];
    u16 amountPerStep, selectedSteps;
};
s32 Div(s32, s32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
void DarkLoan_UpdateAdjustmentDigits(struct DarkLoanAdjustment *display)
{
    u8 *p = (u8 *)display;
    s32 digits[4];
    s32 value, remaining, i;
    s32 *digit, *quantityDigit;
    void *data;
    u8 *base, *anim;
    u32 *flags;

    value = display->selectedSteps;
    if (value == 0) {
        base = p + 0x60;
        data = display->resource;
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
        base = p + 0x60;
        data = display->resource;
    }
    digit = digits;
    anim = (u8 *)display->digits;
    flags = (u32 *)(base + 0x3C0);
    i = 3;
    do {
        *flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(*digit++ + 8), 1);
        anim += 0x60;
        flags = (void *)((u8 *)flags + 0x60);
    } while (--i >= 0);
    display->separator.flags &= ~1;
    value = display->selectedSteps * display->amountPerStep;
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
    quantityDigit = digits;
    anim = (u8 *)display->amountDigits;
    flags = (u32 *)(base + 0x5A0);
    i = 3;
    do {
        *flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(*quantityDigit++ + 8), 1);
        anim += 0x60;
        flags = (void *)((u8 *)flags + 0x60);
    } while (--i >= 0);
}
