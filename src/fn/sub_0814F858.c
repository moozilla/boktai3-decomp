#include "global.h"

void PlayerCue_BeginAcknowledgement(u8 *p)
{
    p[0x59E] = 2;
    p[0x59F] = 1;
}
