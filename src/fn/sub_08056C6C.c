#include "global.h"

extern u32 gUnk_030053F4;
extern u32 gUnk_0300523C;
void sub_0822B548(void);

void sub_08056C6C(s32 x)
{
    gUnk_030053F4 |= 0x200;
    if (x != 0 && !(gUnk_0300523C & 8)) {
        sub_0822B548();
        gUnk_0300523C |= 8;
    }
}
