#include "global.h"

u32 sub_0800950C(u8 *p) {
    u32 r;
    if (p[6] == 0) {
        r = 0;
    } else {
        p[6] = 0;
        r = 1;
    }
    return r;
}
