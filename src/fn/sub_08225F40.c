#include "global.h"
extern u8 *gUnk_03005420;

void sub_08225F40(void)
{
    if (gUnk_03005420 != NULL)
        *(u16 *)(gUnk_03005420 + 0x54) = 0;
}
