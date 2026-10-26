#include "global.h"
#include "ram.h"

u32 MessageWindow_IsCreated(void)
{
    u32 x = gMessageWindow;
    if (x != 0)
        x = 1;
    return x;
}
