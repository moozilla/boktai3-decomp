#include "global.h"
s32 Div(s32, s32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
#define DarkLoan_UpdateAdjustmentDigits sub_081BC468
void DarkLoan_UpdateAdjustmentDigits(u8 *p)
{
    s32 digits[4];
    s32 value, remaining, i;
    s32 *digit, *quantityDigit;
    void *data;
    u8 *base, *anim;
    u32 *flags;

    value = *(u16 *)(p + 0x8EE);
    if (value == 0) {
        base = p + 0x60;
        data = p + 0x18;
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
        data = p + 0x18;
    }
    digit = digits;
    anim = p + 0x418;
    flags = (u32 *)(base + 0x3C0);
    i = 3;
    do {
        *flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(*digit++ + 8), 1);
        anim += 0x60;
        flags = (void *)((u8 *)flags + 0x60);
    } while (--i >= 0);
    *(u32 *)((u8 *)p + 0x5A0) &= ~1;
    value = *(u16 *)(p + 0x8EE) * *(u16 *)(p + 0x8EC);
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
    anim = p + 0x5F8;
    flags = (u32 *)(base + 0x5A0);
    i = 3;
    do {
        *flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(*quantityDigit++ + 8), 1);
        anim += 0x60;
        flags = (void *)((u8 *)flags + 0x60);
    } while (--i >= 0);
}
