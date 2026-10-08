#include "global.h"

u32 sub_08034240(u8 *p) {
    u32 r;
    if (p[27] == 0) {
        r = 0;
    } else {
        p[27] = 0;
        r = 1;
    }
    return r;
}
