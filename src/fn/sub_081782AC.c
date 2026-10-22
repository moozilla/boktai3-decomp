#include "global.h"
extern u8 *gUnk_02000214;

void sub_081782AC(void)
{
    u8 *hud = gUnk_02000214;
    if (hud != NULL) {
        if ((u8)(hud[0xa5e] - 1) <= 1)
            *(u16 *)(hud + 0xa8c) = 4;
    }
}
