#include "global.h"
// MATCHING_CONCESSION: register-pinning
extern struct Background {
    u8 pad[24];
    s16 width;
} gUnk_03004C30;
s16 *sub_0800E3E0(int, int, int);
int Div(int, int);
int sub_08228D88(void);
int sub_08228D94(void);
void NumericDisplay_DrawStatusValues(void)
{
    int digits[2];
    s16 *dst;
    int row, col;
    register int *values asm("r12");
    int left = sub_08228D88(), right = sub_08228D94();
    digits[0] = Div(left, 10);
    digits[1] = left - digits[0] * 10;
    dst = sub_0800E3E0(0, 16, 11);
    row = 0;
    values = digits;
    for (; row < 2; row++) {
        int shift = row << 4;
        int *digit = values;
        s16 *out = dst;
        for (col = 0; col < 2; col++) {
            int tile = *digit++ + 32;
            *out++ = shift + tile | -12288;
        }
        dst += gUnk_03004C30.width * 2;
    }

    digits[0] = Div(right, 10);
    digits[1] = right - digits[0] * 10;
    dst = sub_0800E3E0(0, 20, 11);
    row = 0;
    values = digits;
    for (; row < 2;) {
        int next = row + 1;
        row <<= 4;
        {
            int *digit = values;
            s16 *out = dst;
            for (col = 0; col < 2; col++) {
                int tile = *digit++ + 32;
                *out++ = row + tile | -12288;
            }
            dst += gUnk_03004C30.width * 2;
        }
        row = next;
    }
}
