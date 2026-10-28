#include "global.h"
#include "ram.h"
/* Wireless_GetLocalParticipantIndex: Returns the participant index assigned by matching the local RFU ID */
#define Wireless_GetLocalParticipantIndex sub_080369F0


s32 sub_080369F0(void)
{
    if (((u8 *)gWirelessController) == NULL)
        return -1;
    return (s8)((u8 *)gWirelessController)[0x1f];
}
