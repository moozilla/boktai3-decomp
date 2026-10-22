#include "global.h"
extern u8 *gUnk_02000208;

s32 sub_0811E514(void)
{
    s32 count;
    if (gUnk_02000208)
        count = *(s32 *)(gUnk_02000208 + 0x13c);
    else
        count = 0;
    return count;
}
