#include "global.h"

void PlayerCue_OverlayOn(u8 *p, s32 v)
{
    *(u16 *)(p + 0xAE4) = 2;
    *(u16 *)(p + 0xAD6) = v;
}
