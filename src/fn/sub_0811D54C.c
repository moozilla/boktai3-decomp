#include "global.h"
extern u8 *gUnk_02000710;
void sub_0811D54C(void)
{
    s32 i = 0;
    do {
        u8 *g = gUnk_02000710;
        *(s32 *)(g + i * 4 + 0x7c8) = -1;
        *(s16 *)(g + i * 2 + 0x800) = -1;
        i++;
    } while (i <= 11);
}
