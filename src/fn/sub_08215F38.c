#include "global.h"
void sub_08215E24(s32, s32, s32, s32, s32, s32 *, s32, s32);
#define DIGIT(i, t)                                                                                \
    {                                                                                              \
        d = base;                                                                                  \
        while (value > (t) - 1) {                                                                  \
            d++;                                                                                   \
            value -= (t);                                                                          \
        }                                                                                          \
        digits[n] = d;                                                                             \
        n++;                                                                                       \
    }
void Text_DrawNumber(s32 layer, s32 x, s32 y, u32 value, s32 base, s32 count, s32 arg7)
{
    s32 digits[8];
    s32 d;
    s32 n;
    s32 rest;
    if (count > 8)
        count = 8;
    n = 0;
    DIGIT(0, 10000000)
    DIGIT(1, 1000000)
    DIGIT(2, 100000)
    DIGIT(3, 10000)
    DIGIT(4, 1000)
    DIGIT(5, 100)
    DIGIT(6, 10)
    DIGIT(7, 1)
    rest = n - count;
    sub_08215E24(layer, x, y, count, 1, digits + rest, 0, arg7);
}
