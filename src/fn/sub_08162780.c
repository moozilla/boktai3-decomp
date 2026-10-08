#include "global.h"
struct S { u8 f[0x77c]; s32 x; };
extern struct S *gUnk_02000710;
void sub_08162780(void)
{
    if (gUnk_02000710->x <= 1)
        gUnk_02000710->x = 0;
    else
        gUnk_02000710->x--;
}
