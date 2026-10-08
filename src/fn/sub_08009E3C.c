#include "global.h"

u8 *sub_08009E3C(u8 *p) {
    s32 i = 0;
    u8 *q = p + 0x2C;
    while (i <= 15) {
        if (q[5] == 0) {
            return q;
        }
        q += 0xA0;
        i++;
    }
    return 0;
}
