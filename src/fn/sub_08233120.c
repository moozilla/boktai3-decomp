#include "global.h"

u8 *sub_08233120(u8 *p) {
    u8 *q = p + 0x18;
    s32 i = 0;
    while (i <= 15) {
        if (q[0] == 0) {
            return q;
        }
        i++;
        q += 0x4C;
    }
    return 0;
}
