#include "global.h"
void Storage_GetSlotPosition(u8 *, u32);
void Storage_UpdateCursorPosition(u8 *a, u8 *b)
{
    Storage_GetSlotPosition(b + 0x20, a[3]);
}
