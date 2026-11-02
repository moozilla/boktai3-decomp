#include "global.h"
struct DigitWidget {
    u8 p0[8];
    u32 flags;
    u8 pc[0x14];
    s16 x, y;
    u8 p24[0x3c];
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_DrawDecimalDigits(struct DigitWidget *widget, void *graphics, s32 value, s32 width,
                            s32 baseFrame, s32 x, s32 y, s32 spacing, u32 suppressZero)
{
    s32 digits[8];
    s32 digitCount, digitIndex, digitFrame;
    s32 firstDigit, screenX;
    u32 leading;

    if (width > 8)
        width = 8;

    digitCount = 0;
#define DIGIT(divisor)                                                                             \
    digitFrame = baseFrame;                                                                        \
    while (value > (divisor) - 1) {                                                                \
        ++digitFrame;                                                                              \
        value -= (divisor);                                                                        \
    }                                                                                              \
    digits[digitCount++] = digitFrame;
    DIGIT(10000000)
    DIGIT(1000000)
    DIGIT(100000)
    DIGIT(10000)
    DIGIT(1000)
    DIGIT(100)
    DIGIT(10)
    DIGIT(1)
#undef DIGIT
    firstDigit = digitCount - width;
    leading = ((0u - suppressZero) | suppressZero) >> 31;
    screenX = x;
    for (digitIndex = 0; digitIndex < width; screenX += spacing, digitIndex++, widget++) {
        if (leading) {
            if (digits[firstDigit + digitIndex] != 0) {
                leading = 0;
            }
        }
        if (leading) {
            widget->flags |= 1;
        } else {
            widget->flags &= ~1;
            Anim_LoadKeyframe(widget, graphics, (u16)digits[firstDigit + digitIndex], 0);
            widget->x = screenX;
            widget->y = y;
        }
    }
}