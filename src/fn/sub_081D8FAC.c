#include "global.h"

u32 sub_081D8FAC(u8 *p) {
    u32 two = 2;
    *(u32 *)(p + 4) |= two;
    if (*(u32 *)p >= *(u16 *)(p + 0x1A)) {
        return 2;
    }
    return 0;
}
