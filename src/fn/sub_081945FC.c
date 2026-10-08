#include "global.h"

void sub_081945FC(u8 *p) {
    u8 *q = p + 0x71BC;
    if (*q == 4) {
        u32 z = 0;
        *q = 5;
        *(u16 *)(p + 0x71C0) = z;
    }
}
