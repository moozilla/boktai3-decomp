#include "global.h"

s32 sub_08049CD8(void);

u32 sub_08072BC4(u8 *p) {
    u32 r;
    if ((p[4] & 2) || *(u16 *)(p + 6) || (p[5] == 1 && sub_08049CD8() == 0)) r = 0;
    else r = 1;
    return r;
}
