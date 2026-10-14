#include "global.h"
void DisplayAnimation_UpdatePlayback(u8 *);
void sub_081D48B0(u8 *p)
{
    u8 *q = p + 0x28;
    s32 i = 4;
    do {
        DisplayAnimation_UpdatePlayback(q);
        q += 0x94;
        i--;
    } while (i >= 0);
    DisplayAnimation_UpdatePlayback(p + 0x30c);
}
