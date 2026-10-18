#include "global.h"

u32 SunClock_GetPackedDate(void);
void Clock_UnpackDate(u32 *, u32 *, u32 *, u32);

u32 TimeDisplay_GetCurrentMonth(void)
{
    u32 a;
    u32 b;
    u32 c;
    Clock_UnpackDate(&a, &b, &c, SunClock_GetPackedDate());
    return b;
}
