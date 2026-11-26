#include "engine/shared/global.h"
#include "engine/shared/ram.h"


void Wireless_SetStatus(u32 v)
{
    if ((*(u32 *)&gWirelessController) != 0) {
        *(u8 *)((*(u32 *)&gWirelessController) + 0x33) = v;
    }
}
