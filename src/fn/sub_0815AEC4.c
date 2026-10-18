#include "global.h"

void PlayerCue_OverlayOff(u8 *p)
{
    *(u16 *)(p + 0xAE4) = 0;
}
