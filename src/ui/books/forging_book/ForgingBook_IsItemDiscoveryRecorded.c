#include "global.h"
#include "ui/books/forging_book.h"

u32 ForgingBook_IsItemDiscoveryRecorded(s32 index)
{
    if (index <= 31)
        return gGameState->flags720 & (1 << index);
    return gGameState->flags724 & (1 << (index - 32));
}
