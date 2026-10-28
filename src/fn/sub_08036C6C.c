#include "global.h"
#include "ram.h"
/* Wireless_GetLocalIdentityData: Tentatively returns the local identity storage beginning at controller offset 0x50 */
#define Wireless_GetLocalIdentityData sub_08036C6C


u8 *sub_08036C6C(void)
{
    if (((u8 *)gWirelessController) == NULL)
        return NULL;
    return ((u8 *)gWirelessController) + 0x50;
}
