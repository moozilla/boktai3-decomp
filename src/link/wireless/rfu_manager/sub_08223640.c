#include "global.h"

void RfuManager_ResetConnectionState(void);

void RfuManager_StartConfiguration(u8 *q)
{
    u8 *p;
    RfuManager_ResetConnectionState();
    p = (u8 *)0x03005390;
    p[4] = 1;
    p[5] = 2;
    *(u8 **)(p + 0x3c) = q;
    p[9] = q[0x11];
    *(u16 *)(p + 0x32) = *(u16 *)(q + 0x12);
    *(u16 *)(p + 0x18) = *(u16 *)(q + 0x14);
    if (q[0x10] != 0)
        p[0xb] = 1;
}
