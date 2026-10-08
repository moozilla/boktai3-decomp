#include "global.h"

void sub_08224980(u32, u32);

void sub_08224C84(void) {
    u8 *p = (u8 *)0x03005390;
    if (p[2] != 0) {
        ((volatile u8 *)p)[2];
        p[2] = 0;
        sub_08224980(0x45, 0);
    }
}
