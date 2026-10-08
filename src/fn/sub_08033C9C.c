#include "global.h"

u32 sub_08033C9C(u8 *p) {
    u32 r;
    if (p[25] == 0) {
        r = 0;
    } else {
        p[25] = 0;
        r = 1;
    }
    return r;
}
