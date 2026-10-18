#include "global.h"
u16 *Video_GetWrappedBackgroundMapEntry(s32, u8 *, u32);
void Video_DrawBackgroundRowMarkers(u8 *a, u32 b)
{
    u16 *p = Video_GetWrappedBackgroundMapEntry(3, a, b);
    s32 i;
    for (i = 0; i <= 6; i++) {
        if (i != 3)
            *p = 0x10;
        p++;
    }
    *p = 1;
}
