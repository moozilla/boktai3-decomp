#include "global.h"
extern u8 *gGameState;

void sub_08211860(s32 bit)
{
    if (bit <= 31)
        *(u32 *)(gGameState + 0x720) |= 1 << bit;
    else
        *(u32 *)(gGameState + 0x724) |= 1 << (bit - 32);
}
