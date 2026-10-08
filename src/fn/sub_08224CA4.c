#include "global.h"

void sub_08224980(u32, u32);

void sub_08224CA4(void) {
    volatile u8 *p = (volatile u8 *)0x03005390;
    if (p[2] == 0) {
        sub_08224980(0x45, 0);
    } else if (p[2] == 1) {
        p[2];
        p[2] = 2;
    }
}
