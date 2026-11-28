#include "global.h"
#include "engine/input.h"
#include "gba/io_reg.h"

void sub_081BB248(u16 *cursor)
{
    if (gPadState[0] & DPAD_RIGHT)
        cursor[0]++;
    else if (gPadState[0] & DPAD_LEFT)
        cursor[0]--;
    if (gPadState[0] & DPAD_UP)
        cursor[1]--;
    else if (gPadState[0] & DPAD_DOWN)
        cursor[1]++;
}
