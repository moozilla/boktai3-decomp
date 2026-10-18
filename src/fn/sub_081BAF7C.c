#include "global.h"
void MenuCursor_OnSelectionFlag(u8 *p)
{
    *(u32 *)(p + 0x1A4) |= 2;
}
