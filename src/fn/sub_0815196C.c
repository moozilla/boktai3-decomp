#include "global.h"

void PlayerCue_UpdateRunOverlay(u8 *p)
{
    u16 *q = (u16 *)(p + 0xAD6);
    u16 v = 0xBA;

    v <<= 1;
    *q = v;
    *(u16 *)(p + 0xAD8) = 0x18;
}
