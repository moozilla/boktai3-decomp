#include "global.h"
extern u32 *gUnk_030025F8;
u32 *sub_08225AA0(void)
{
    if (gUnk_030025F8 == 0)
        return 0;
    return gUnk_030025F8;
}
