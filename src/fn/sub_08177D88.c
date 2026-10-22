#include "global.h"
extern u8 *gUnk_02000214;

s32 sub_08177D88(void)
{
    s32 result;
    if (gUnk_02000214 != NULL)
        result = *(u16 *)(gUnk_02000214 + 0xa5c);
    else
        result = -1;
    return result;
}
