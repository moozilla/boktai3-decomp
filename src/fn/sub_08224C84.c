#include "global.h"

void RfuManager_NotifyEvent(u32, u32);

void RfuManager_CompletePendingMscCycle(void)
{
    u8 *p = (u8 *)0x03005390;
    if (p[2] != 0) {
        ((volatile u8 *)p)[2];
        p[2] = 0;
        RfuManager_NotifyEvent(0x45, 0);
    }
}
