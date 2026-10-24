// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct DigitWidget {
    u8 p0[8];
    u32 flags;
    u8 pc[0x14];
    s16 x, y;
    u8 p24[0x3c];
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_DrawDecimalDigits(struct DigitWidget *w, void *data, s32 value, s32 n, s32 base,
                            s32 initialX, s32 y, s32 spacing, u32 suppressZero)
{
    s32 digits[8];
    s32 count, digit;
    s32 start;
    s32 negative;

    if (n > 8)
        n = 8;
    count = 0;
#define DIGIT(divisor)                                                                             \
    digit = base;                                                                                  \
    while (value > divisor - 1) {                                                                  \
        digit++;                                                                                   \
        value -= divisor;                                                                          \
    }                                                                                              \
    digits[count++] = digit;
    digit = base;
    {
        s32 limit = 9999999;
        negative = -suppressZero;
        while (value > limit) {
            digit++;
            value -= 10000000;
        }
    }
    digits[count++] = digit;
    DIGIT(1000000)
    DIGIT(100000)
    DIGIT(10000)
    DIGIT(1000)
    DIGIT(100)
    DIGIT(10)
    DIGIT(1)
#undef DIGIT
    {
        register s32 i asm("r6");
        s32 x;
        u32 leading;
        s32 *current;
        start = count - n;
        /* Finalize suppression after extracting the decimal digits. */
        asm("" : "+r"(negative));
        leading = (negative | suppressZero) >> 31;
        x = initialX;
        i = 0;
        if (i < n) {
            current = (s32 *)((start * 4) + (u32)digits);
            do {
                s32 index = start + i;
                if (leading) {
                    if (*current != 0)
                        leading = 0;
                }
                if (leading)
                    w->flags |= 1;
                else {
                    w->flags &= ~1;
                    Anim_LoadKeyframe(w, data, (u16)digits[index], 0);
                    w->x = x;
                    w->y = y;
                }
                x += spacing;
                current++;
                i++;
                w++;
            } while (i < n);
        }
    }
}
