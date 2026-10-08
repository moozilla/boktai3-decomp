#include "global.h"

u32 sub_08047700(u8 *p) {
    u32 r;
    if (p[29] == 0) {
        r = 0;
    } else {
        p[29] = 0;
        r = 1;
    }
    return r;
}
