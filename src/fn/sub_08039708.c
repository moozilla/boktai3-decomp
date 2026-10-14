#include "global.h"

u8 *sub_08037DC0(void);

u32 sub_08039708(void)
{
    u8 *p = sub_08037DC0();
    u8 *flag;
    if (p == NULL || *(flag = p + 0x30) == 0)
        return 0;
    *flag = 0;
    return 1;
}
