#include "global.h"
extern u8 *gUnk_02000214;
void sub_0816B92C(void)
{
    if (gUnk_02000214)
        *(u32 *)(gUnk_02000214 + 0x4928) = 1;
}
