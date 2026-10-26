#include "global.h"
#include "ram.h"

void sub_081782AC(void)
{
    u8 *hud = gFieldHud;
    if (hud != NULL) {
        if ((u8)(hud[0xa5e] - 1) <= 1)
            *(u16 *)(hud + 0xa8c) = 4;
    }
}
