#include "global.h"

void sub_081945D0(u8 *p) {
    u8 *q = p + 0x71BC;
    if (*q == 4 || *q == 6) {
        u32 z = 0;
        *q = 7;
        *(u16 *)(p + 0x71C0) = z;
    }
}
