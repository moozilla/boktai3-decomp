#include "global.h"

u32 sub_08194640(u8 *p) {
    u32 r = 0;
    if (*(p + 0x71BC) == 0) {
        r = 1;
    }
    return r;
}
