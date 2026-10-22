#include "global.h"

void sub_08159814(u8 *player, s32 amount)
{
    if (*(s32 *)(player + 0x1c) == 1) {
        u32 flag = player[0x418];
        if (flag == 0) {
            u16 *p = (u16 *)(player + 0x42c);
            if (*p <= amount)
                *p = flag;
            else
                *p -= amount;
        }
    }
}
