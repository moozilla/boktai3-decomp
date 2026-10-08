#include "global.h"

u8 *sub_08011C4C(u8 *p) {
    u8 *q = p + 0x8C;
    s32 i = 0;
    while (i <= 15) {
        if (q[0x10] == 0) {
            return q;
        }
        i++;
        q += 0xB4;
    }
    return 0;
}
