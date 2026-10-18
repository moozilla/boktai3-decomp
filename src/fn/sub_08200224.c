#include "global.h"
extern u8 *gUnk_020005F8;
u32 PlayerClone_GetActiveCount(void)
{
    return *(u32 *)(gUnk_020005F8 + 0x9e8);
}
