#include "global.h"
extern u8 *gUnk_02000710;

void sub_0811D600(u32 a, u32 b)
{
    if (a <= 0xb && b <= 5) {
        u8 *g = gUnk_02000710;
        if (*(s16 *)(g + a * 2 + 0x800) == -1)
            *(s16 *)(g + a * 2 + 0x800) = b;
        else if (*(s16 *)(g + a * 2 + 0x800) > (s32)b)
            *(s16 *)(g + a * 2 + 0x800) = b;
    }
}
