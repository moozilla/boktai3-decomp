#include "global.h"
u32 sub_08036490(u8 *p) {
    u8 *q = p + 0x32;
    if (*q) *q = 0;
    return 0;
}
