#include "global.h"

u32 sub_08224F2C(void) {
    u32 m = 0x400;
    if (((*(u32 *)0x030053F4 | *(u32 *)0x030053F0) & m) == 0
        && *(u32 *)0x03005400 == 0
        && *(u32 *)0x030053E0 == 0)
        return 0;
    return 1;
}
