#include "global.h"

void sub_082221E8(u8 *a, u32 *b) {
    u32 y = b[1];
    u32 x = b[0];
    *(u32 *)(a + 0x20) = x;
    *(u32 *)(a + 0x24) = y;
}
