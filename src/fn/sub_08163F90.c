#include "global.h"

s32 sub_08163F90(s32 a, s32 b)
{
    s32 r = 0x11;
    if (b != 0) {
        r = 0x31;
        if (b == 1)
            r = 0x21;
    }
    return a + r;
}
