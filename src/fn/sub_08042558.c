#include "global.h"

u32 sub_08214514(u8 *);
u32 sub_082195E0(u8 *);

s32 sub_08042558(u8 *p) {
    u32 mode = *(u32 *)p;
    if (mode == 0) {
        return -1;
    }
    if (mode == 1) {
        sub_08214514(p + 0x28);
    } else {
        sub_082195E0(p + 0x2c);
    }
    *(u32 *)p = 0;
    return 0;
}
