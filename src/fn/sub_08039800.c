#include "global.h"
/* WirelessGame_IsRoundActive: Tests whether the controller is in gameplay state 7 */
#define WirelessGame_IsRoundActive sub_08039800

u8 *WirelessExchange_Get(void);

u32 sub_08039800(void)
{
    u8 *p = WirelessExchange_Get();
    if (p == NULL || p[0x28] != 7)
        return 0;
    return 1;
}
