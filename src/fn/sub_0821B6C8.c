#include "global.h"
s32 Div(s32, s32);
void sub_0821B6C8(u8 *output, u8 *input)
{
    u8 *src = input;
    u8 *dest = output;
    while (*src) {
        if (*src & 0x80) {
            s32 row = *src - 0x80;
            s32 column = src[1];
            s32 adjusted;
            src++;
            if (row & 1) {
                adjusted = column - 0x61;
                if (adjusted > 0x7E)
                    adjusted++;
            } else
                adjusted = column - 2;
            row = Div(row - 0x21, 2) + 0x81;
            if (row > 0x9F)
                row += 0x40;
            dest[0] = row;
            dest[1] = adjusted;
            dest++;
        } else
            *dest = *src;
        if (src[0] == 13 && src[1] == 10)
            src++;
        src++;
        dest++;
    }
    *dest = 0;
}
