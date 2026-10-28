#include "global.h"
#include "input.h"
void TimeResult_BeginFadeOut(void *);
s32 sub_081C5FC4(u8 *p)
{
    u16 *k = gPadState;
    if (k[0] & 0x20)
        (*(u16 *)(p + 0x374))--;
    else if (k[0] & 0x10)
        (*(u16 *)(p + 0x374))++;
    if (k[0] & 0x40)
        (*(u16 *)(p + 0x376))--;
    else if (k[0] & 0x80)
        (*(u16 *)(p + 0x376))++;
    if (k[1] & 1)
        TimeResult_BeginFadeOut(p);
}
