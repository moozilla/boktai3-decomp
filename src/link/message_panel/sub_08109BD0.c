#include "global.h"

void ClockPanel_CaptureSnapshot(u8 *);
u32 SunClock_GetPhase(void);

void Registry1745_InitSessionSettings(u8 *p)
{
    ClockPanel_CaptureSnapshot(p);
    *(u8 *)(p + 0x20C) = 0;
    *(u8 *)(p + 0x11D) = SunClock_GetPhase();
}
