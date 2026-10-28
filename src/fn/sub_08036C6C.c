#include "global.h"
/* Wireless_GetLocalIdentityData: Tentatively returns the local identity storage beginning at controller offset 0x50 */
#define Wireless_GetLocalIdentityData sub_08036C6C

extern u8 *gWirelessController;

u8 *sub_08036C6C(void)
{
    if (gWirelessController == NULL)
        return NULL;
    return gWirelessController + 0x50;
}
