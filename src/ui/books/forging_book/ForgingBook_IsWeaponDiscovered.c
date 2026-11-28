#include "global.h"
#include "ui/books/forging_book.h"

u32 ForgingBook_IsWeaponDiscovered(s32 index)
{
    if (index <= 31)
        return gGameState->flags72C & (1 << index);
    return gGameState->flags730 & (1 << (index - 32));
}
