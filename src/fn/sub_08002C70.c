#include "palette_controller.h"

u32 sub_08002C70(void)
{
    u8 v = gUnk_03005430.state - 1;
    if (v <= 2) {
        return 0x20;
    }
    return 0x40;
}
