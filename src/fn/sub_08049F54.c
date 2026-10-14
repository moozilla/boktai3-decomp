#include "global.h"

extern u8 *gUnk_02000488;

u16 sub_08049F54(void)
{
    if (gUnk_02000488 == NULL)
        return 0;
    return *(u16 *)(gUnk_02000488 + 0x3e6);
}
