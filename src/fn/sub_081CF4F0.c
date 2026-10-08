#include "global.h"
s32 sub_081CEDD4(u8 *, u32, u32);
static inline void orr(u16 *a, u32 m) { *a = m | *a; }
void sub_081CF4F0(u8 *a, u8 *b, u32 c)
{
    if (sub_081CEDD4(b, c, 0x10)) {
        orr((u16 *)(b + 0xe), 0x100);
        b[7] = 1;
    }
}
