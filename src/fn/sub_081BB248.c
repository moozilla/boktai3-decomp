#include "global.h"
#include "input.h"

void sub_081BB248(u16 *cursor)
{
    if (gPadState[0] & 0x10)
        cursor[0]++;
    else if (gPadState[0] & 0x20)
        cursor[0]--;
    if (gPadState[0] & 0x40)
        cursor[1]--;
    else if (gPadState[0] & 0x80)
        cursor[1]++;
}
