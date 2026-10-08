#include "global.h"

u32 sub_08045D3C(u8 *p) {
    u32 r;
    if (p[2] == 0) {
        r = 0;
    } else {
        p[2] = 0;
        r = 1;
    }
    return r;
}
