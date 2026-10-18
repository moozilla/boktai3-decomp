#include "global.h"

extern u32 gUnk_02000484;

void Wireless_SetStatus(u32 v)
{
    if (gUnk_02000484 != 0) {
        *(u8 *)(gUnk_02000484 + 0x33) = v;
    }
}
