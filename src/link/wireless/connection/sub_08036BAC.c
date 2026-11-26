#include "global.h"
#include "engine/ram.h"
/* Wireless_CopyReceivedNiPacket: Copies the twelve-byte NI receive buffer into the caller destination */


s32 Wireless_CopyReceivedNiPacket(void *dst)
{
    if (((u8 *)gWirelessController) == NULL)
        return -1;
    CpuSet(((u8 *)gWirelessController) + 0xc4, dst, 0x04000003);
    return 0;
}
