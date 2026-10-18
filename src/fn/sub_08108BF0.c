#include "global.h"

extern void SerialLink_ResetMultiplayer(void);
void SlotMonitor_PollResult(void);

void LinkHandshake_UpdateRestartState(u8 *p)
{
    u8 *r4 = p + 0x6E;
    u32 v = *r4;

    if (v != 0) {
        SerialLink_ResetMultiplayer();
        *r4 = 0;
    } else {
        *(void **)(p + 0x7C) = SlotMonitor_PollResult;
        *(u32 *)(p + 0x50) = v;
        *(u32 *)(p + 0x3C) = v;
        *(u32 *)(p + 0x40) = v;
        *(u32 *)(p + 0x58) = v;
    }
}
