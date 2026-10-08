#include "global.h"

u32 sub_08220C8C(u8 *, u32, u16, u8, u8);

u32 sub_08220D20(u8 *p, u32 a, u16 b, u8 c, u8 d) {
    p[5] = 0;
    return sub_08220C8C(p, a, b, c, d);
}
