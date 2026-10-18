// COMPILER: old_agbcc
#include "global.h"
/* WirelessGame_SuppressPlayerReset: Sets global flag 0x1000 which suppresses a player reset call; interpretation tentative */
#define WirelessGame_SuppressPlayerReset sub_080397C0

extern u32 gUnk_030053F4;

void sub_080397C0(void)
{
    gUnk_030053F4 |= 0x1000;
}
