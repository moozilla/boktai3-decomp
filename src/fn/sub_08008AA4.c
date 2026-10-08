#include "global.h"

u32 sub_08008AA4(u8 *p) {
    u32 r;
    if (p[4] == 0) {
        r = 0;
    } else {
        p[4] = 0;
        r = 1;
    }
    return r;
}
