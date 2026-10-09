#include "global.h"
u16 *sub_08163F70(s32, s32, s32);
void sub_0816C1A4(void) {
    s32 y;
    s32 x;
    for (y = 0; y <= 0x1f; y++) {
        for (x = 0; x <= 0x1f; x++)
            *sub_08163F70(1, x, y) = 0xE002;
    }
}
