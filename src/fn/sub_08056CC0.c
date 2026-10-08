#include "global.h"

extern u32 gUnk_030053F4;
extern u32 gUnk_0300523C;
void sub_0822B560(void);

void sub_08056CC0(void)
{
    gUnk_030053F4 &= ~0x200;
    if (gUnk_0300523C & 8) {
        sub_0822B560();
        { u32 m = ~8; gUnk_0300523C &= m; }
    }
}
