#include "global.h"

u8 *sub_08013C2C(u8 *p, u32 key) {
    u8 *q = *(u8 **)(p + 0x18);
    while (q) {
        if (*(u16 *)q == key) {
            return q;
        }
        q = *(u8 **)(q + 0x50);
    }
    return 0;
}
