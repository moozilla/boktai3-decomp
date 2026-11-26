#include "global.h"
extern u8 *gGameState;
void sub_08211820(s32 bit)
{
    if (bit <= 31)
        *(u32 *)(gGameState + 0x72c) |= 1 << bit;
    else
        *(u32 *)(gGameState + 0x730) |= 1 << (bit - 32);
}