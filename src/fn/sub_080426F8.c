#include "global.h"

s32 sub_080426F8(u8 *p, u32 mask) {
    u32 *q;
    if (*(u32 *)p == 0) {
        return -1;
    }
    q = *(u32 **)(p + 0x90);
    *q &= ~mask;
    return 0;
}
