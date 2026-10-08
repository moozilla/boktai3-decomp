#include "global.h"

void sub_08083E70(u16 a, s32 *b, s32 *c) {
    s32 t = ((a + 0x70) >> 5) & 7;
    if (t > 4) {
        *b = 8 - t;
        *c = 1;
    } else {
        *b = t;
        *c = 0;
    }
}
