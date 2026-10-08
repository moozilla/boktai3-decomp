#include "global.h"

s32 sub_080427C0(u8 *p, u32 v) {
    u32 mode = *(u32 *)p;
    if (mode == 0) {
        return -1;
    }
    if (mode == 1) {
        *(u32 *)(p + 0x18) = v;
    } else {
        *(u32 *)(p + 0x74) = v;
    }
    return 0;
}
