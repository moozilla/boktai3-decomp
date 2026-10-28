#include "global.h"
/* Wireless_GetLocalParticipantIndex: Returns the participant index assigned by matching the local RFU ID */
#define Wireless_GetLocalParticipantIndex sub_080369F0

extern u8 *gWirelessController;

s32 sub_080369F0(void)
{
    if (gWirelessController == NULL)
        return -1;
    return (s8)gWirelessController[0x1f];
}
