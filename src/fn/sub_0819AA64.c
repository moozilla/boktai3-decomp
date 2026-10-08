#include "global.h"

u32 sub_081FCFA8(u8 *);
u32 sub_081FE6E0(u32);

s32 sub_0819AA64(u8 *p) {
    u32 r = sub_081FCFA8(p);
    *(u32 *)(p + 0x18) = r;
    if (r != 0) {
        r = sub_081FE6E0(r);
        *(u32 *)(p + 0x1C) = r;
        if (r != 0) {
            return 0;
        }
    }
    return -1;
}
