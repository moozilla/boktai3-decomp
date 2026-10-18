#include "global.h"

void RfuManager_NotifyEvent(u32, u32);

void RfuManager_RequestMscCycleCompletion(void)
{
    volatile u8 *p = (volatile u8 *)0x03005390;
    if (p[2] == 0) {
        RfuManager_NotifyEvent(0x45, 0);
    } else if (p[2] == 1) {
        p[2];
        p[2] = 2;
    }
}
