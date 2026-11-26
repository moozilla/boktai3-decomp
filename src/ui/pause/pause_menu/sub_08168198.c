#include "global.h"
u8 PauseMenu_GetSelectedListEntryType(u8 *p)
{
    s8 i = *(s8 *)(p + 0xF48);
    u8 *q = p + i * 8;
    q += 0xDAA;
    return *q;
}
