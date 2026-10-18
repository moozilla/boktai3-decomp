#include "global.h"

extern u8 *gUnk_02000484;

u32 Wireless_IsSessionActive(void)
{
    return gUnk_02000484 ? gUnk_02000484[0x30] : 0x0;
}
