#include "global.h"
#include "player.h"
void SegmentDisplay_UpdateScreenBrightness(void *unused)
{
    u16 *count = &gPlayers[0]->displaySegmentCount;
    if (*count <= 3) {
        *(vu16 *)0x04000050 = 0xee;
        *(vu16 *)0x04000054 = (4 - *count) * 2;
    } else {
        *(vu16 *)0x04000050 = 0xae;
        *(vu16 *)0x04000054 = *count - 4;
    }
    *(vu16 *)0x04000052 = 0;
}
