#include "global.h"

void PlayerCue_OverlaySet(u8 *p, s32 v)
{
    *(u16 *)(p + 0xAE4) = 1;
    *(u16 *)(p + 0xAD6) = v;
}
