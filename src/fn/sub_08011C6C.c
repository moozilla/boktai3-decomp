#include "global.h"

u8 *sub_08011C6C(u8 *p, u32 key) {
    u8 *q = p + 0x8C;
    s32 i = 0;
    while (i <= 15) {
        if (q[0x10] != 0 && *(u32 *)(q + 0xC) == key) {
            return q;
        }
        i++;
        q += 0xB4;
    }
    return 0;
}
