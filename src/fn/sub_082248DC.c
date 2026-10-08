#include "global.h"

void sub_082248DC(void) {
    u8 *p = (u8 *)0x03005390;
    if (p[6] == 0 && p[0xa] == 1) {
        p[0x11] = p[4];
        p[0x12] = p[5];
        p[4] = 0x10;
        p[5] = 0x11;
        p[0xa] = 2;
    }
}
