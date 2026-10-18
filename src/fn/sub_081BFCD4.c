#include "global.h"
void sub_081BFD00(void);
void Storage_SetUpdateCallback(u8 *, void (*)(void));
void Storage_UpdateOpeningDelay(u8 *p)
{
    u16 *c = (u16 *)(p + 0x174E);
    (*c)++;
    if (*c > 0x1f)
        Storage_SetUpdateCallback(p, sub_081BFD00);
}
