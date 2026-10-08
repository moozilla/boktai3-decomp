#include "global.h"

u32 sub_08012D20(u8 *p) {
    u8 *q = p + 0x9F;
    u32 r;
    if (*q == 0) {
        r = 0;
    } else {
        *q = 0;
        r = 1;
    }
    return r;
}
