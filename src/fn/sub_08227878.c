#include "global.h"

u8 *PlayerName_GetBuffer(void)
{
    return *(u8 **)0x02000710 + 0x780;
}
