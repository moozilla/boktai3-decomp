#include "global.h"

s32 sub_08011578(u8 *p, u32 a, u32 b) {
    s32 i = 0;
    while (i <= 0x3F) {
        if (p[0] == a && p[1] == b) {
            return i;
        }
        i++;
        p += 2;
    }
    return -1;
}
