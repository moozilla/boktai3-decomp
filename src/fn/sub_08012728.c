#include "global.h"

u8 *sub_08012728(u8 *p) {
    u8 *q = p + 0x38;
    s32 i = 0;
    while (i <= 7) {
        if (q[0] == 0) {
            return q;
        }
        i++;
        q += 0xA0;
    }
    return 0;
}
