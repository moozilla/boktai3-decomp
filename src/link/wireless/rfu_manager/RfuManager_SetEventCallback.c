#include "global.h"

void RfuManager_SetEventCallback(u32 a)
{
    u8 *p = (u8 *)0x03005390;
    *(u32 *)(p + 0x40) = a;
}
