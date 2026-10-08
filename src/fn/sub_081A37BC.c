#include "global.h"

u32 sub_082215E4(s32, s32);

u32 sub_081A37BC(u8 *p, u8 *q) {
    s32 dx = *(s16 *)q - *(s16 *)(p + 0x1C);
    s32 dy = *(s16 *)(q + 4) - *(s16 *)(p + 0x20);
    return sub_082215E4(dx, dy);
}
