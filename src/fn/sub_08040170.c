#include "global.h"
void sub_08215EB4(s32, s32, s32, s32, s32, s32, s32, s32);
void sub_08215DF0(s32, s32, s32, s32, s32, s32);
void sub_08040170(s32 a, s32 b, s32 count, s32 y, s32 width)
{
    s32 diff;
    s32 color;
    s32 i;
    diff = count - b;
    sub_08215EB4(2, y, width, 4, 1, 1, 0, 0);
    color = 0x3b;
    if (diff <= 0) {
        color = 0x1b;
        if (diff < 0) color = 0x2b;
    }
    i = 0;
    if (i < count) {
        do {
            s32 c;
            c = color;
            if (i >= a) goto no_increment;
            c++;
        no_increment:
            sub_08215DF0(2, y + i, width, c, 0, 0);
            i++;
        } while (i < count);
    }
}
