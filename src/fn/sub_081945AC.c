#include "global.h"

void sub_081945AC(u8 *p) {
    u8 *q = p + 0x71BC;
    u8 v = *q;
    if (v == 0) {
        *q = 1;
        *(u16 *)(p + 0x71C0) = v;
    }
}
