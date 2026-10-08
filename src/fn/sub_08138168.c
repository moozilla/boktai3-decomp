#include "global.h"

u32 sub_08138168(u8 *p) {
    u8 *q = p + 0x22;
    u32 r;
    if (*q == 0) {
        r = 0;
    } else {
        *q = 0;
        r = 1;
    }
    return r;
}
