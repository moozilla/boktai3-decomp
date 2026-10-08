#include "global.h"
s32 sub_082118A0(s32);
s32 sub_082117C4(void)
{
    s32 i;
    for (i = 0; i <= 0x30; i++) {
        if (sub_082118A0(i) == 0)
            return 0;
    }
    return 1;
}
