#include "global.h"

u32 sub_08185240(u8 *p, u32 v) {
    u8 *a = p + 0xB3C;
    if (*a != v) {
        u8 *b = p + 0xB3D;
        if (*b == 0xff) {
            u32 z = 0;
            *a = v;
            *b = z;
            *(p + 0xB3E) = z;
            return 1;
        }
    }
    return 0;
}
