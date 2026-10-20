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
#ifdef NONMATCHING
// fuzzy: 94.9%
void Anim_DrawDecimalDigits(struct DigitWidget *w, void *data, s32 value, s32 n, s32 base,
                            s32 initialX, s32 y, s32 spacing, volatile u32 suppressZero)
{
    s32 digits[8];
    s32 count, digit;
    register s32 width asm("sl") = n;
    register s32 start asm("sb");
    s32 negative;

    if (width > 8)
        width = 8;
    count = 0;
    negative = -suppressZero;
#define DIGIT(divisor)                                                                             \
    digit = base;                                                                                  \
    while (value > divisor - 1) {                                                                  \
        digit++;                                                                                   \
        value -= divisor;                                                                          \
    }                                                                                              \
    digits[count++] = digit;
    DIGIT(10000000)
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
        register u32 leading asm("r8");
        s32 x;
        s32 *current;
        start = count - width;
        leading = (negative | suppressZero) >> 31;
        x = initialX;
        i = 0;
        if (i < width) {
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
            } while (i < width);
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Anim_DrawDecimalDigits);
#endif
