#include "global.h"
extern u8 *gUnk_02000118;
void sub_08052628(u8 *, void (*)(void));
void sub_080528D0(void);
void sub_080537B4(void)
{
    if (gUnk_02000118)
        sub_08052628(gUnk_02000118, sub_080528D0);
}
