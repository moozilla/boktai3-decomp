#include "global.h"

u32 sub_08184FE8(u8 *, u32);

u32 sub_081851C4(u8 *p, u32 v) {
    s32 d;
    *(p + 0xB3A) = v;
    d = (*(p + 0xB3A) - *(p + 0xB39)) & 0xff;
    if (d <= 0x80) {
        return sub_08184FE8(p, 0x1a);
    }
    return sub_08184FE8(p, 0x1b);
}
