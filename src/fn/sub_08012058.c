#include "global.h"

u32 sub_08012058(u32 unused, u8 *p) {
    u32 r;
    if (p[2] == 0) {
        r = 0;
    } else {
        p[2] = 0;
        r = 1;
    }
    return r;
}
