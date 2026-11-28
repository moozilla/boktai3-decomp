#include "global.h"
#include "ui/books/forging_book.h"

u32 ForgingBook_IsItemDiscovered(s32 item)
{
    u32 low = gGameState->flags720 | 0x04000400;
    u32 high = gGameState->flags724 | 0x8020;

    if (item <= 31)
        return low & (1 << item);
    return high & (1 << (item - 32));
}
