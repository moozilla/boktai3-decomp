#include "global.h"
/* Wireless_CopyReceivedNiPacket: Copies the twelve-byte NI receive buffer into the caller destination */
#define Wireless_CopyReceivedNiPacket sub_08036BAC

extern u8 *gWirelessController;

s32 sub_08036BAC(void *dst)
{
    if (gWirelessController == NULL)
        return -1;
    CpuSet(gWirelessController + 0xc4, dst, 0x04000003);
    return 0;
}
