#include "global.h"

void sub_08020D68(u8 *, s32);

void sub_0814F874(u8 *p)
{
    u8 *flag = p + 0x59F;

    if (*flag != 0) {
        sub_08020D68(p + 0x2F8, 1);
        *flag = 0;
    }
}
