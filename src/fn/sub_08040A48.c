#include "global.h"

void sub_082195E0(u8 *);

u32 sub_08040A48(u8 *p) {
    u8 *e = p + 0x17C;
    s32 i = 0x11;
    do {
        sub_082195E0(e);
        i--;
        e += 0x60;
    } while (i >= 0);
    return 0;
}
