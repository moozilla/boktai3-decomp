#include "global.h"
/* WirelessGame_ConsumePendingEvent: Returns and clears the pending event flag at offset 0x30 */
#define WirelessGame_ConsumePendingEvent sub_08039708

u8 *WirelessExchange_Get(void);

u32 sub_08039708(void)
{
    u8 *p = WirelessExchange_Get();
    u8 *flag;
    if (p == NULL || *(flag = p + 0x30) == 0)
        return 0;
    *flag = 0;
    return 1;
}
