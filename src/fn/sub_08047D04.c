#include "global.h"

u8 *sub_08047D04(u8 *p) {
    u8 *q = p + 0x38;
    s32 i = 0;
    while (i <= 15) {
        if (q[0] == 0) {
            return q;
        }
        i++;
        q += 0x64;
    }
    return 0;
}
