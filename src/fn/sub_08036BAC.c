#include "global.h"
#include "ram.h"
/* Wireless_CopyReceivedNiPacket: Copies the twelve-byte NI receive buffer into the caller destination */
#define Wireless_CopyReceivedNiPacket sub_08036BAC


s32 sub_08036BAC(void *dst)
{
    if (((u8 *)gWirelessController) == NULL)
        return -1;
    CpuSet(((u8 *)gWirelessController) + 0xc4, dst, 0x04000003);
    return 0;
}
