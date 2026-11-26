#include "global.h"
#include "engine/ram.h"
/* Wireless_GetLocalParticipantIndex: Returns the participant index assigned by matching the local RFU ID */


s32 Wireless_GetLocalParticipantIndex(void)
{
    if (((u8 *)gWirelessController) == NULL)
        return -1;
    return (s8)((u8 *)gWirelessController)[0x1f];
}
