#include "global.h"
#include "engine/ram.h"
/* Wireless_GetLocalIdentityData: Tentatively returns the local identity storage beginning at controller offset 0x50 */


u8 *Wireless_GetLocalIdentityData(void)
{
    if (((u8 *)gWirelessController) == NULL)
        return NULL;
    return ((u8 *)gWirelessController) + 0x50;
}
