#include "global.h"

u8 *sub_080197F8(u8 *p) {
    u8 *q = p + 0x1C;
    s32 i = 0;
    while (i <= 7) {
        if (q[0x4] == 0) {
            return q;
        }
        i++;
        q += 0x3C;
    }
    return 0;
}
